#include "SNSegmentor.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <set>
#include "SNImageDebugger.h"
//---------------------------------------------------
namespace SNNumberRecognizer
{
	SNSegmentor::SNSegmentor()
	{
		PointsCount = 0;

		PixAround[0] = std::make_pair(0, 1);
		PixAround[1] = std::make_pair(-1, 0);
		PixAround[2] = std::make_pair(1, 0);
		PixAround[3] = std::make_pair(0, -1);
		PixAround[4] = std::make_pair(-1, -1);
		PixAround[5] = std::make_pair(-1, 1);
		PixAround[6] = std::make_pair(1, 1);
		PixAround[7] = std::make_pair(1, -1);
	}
	//---------------------------------------------------

	SNSegmentor::~SNSegmentor()
	{
	}
	//---------------------------------------------------

	void SNSegmentor::Segment(SNPlate& plate, int32_t min_gray_level /*= 0*/, int32_t max_gray_level /*= 255*/, int32_t gray_step /*= -1*/)
	{
		cv::Mat norm_gray_image;

		//cv::equalizeHist(gray_image, norm_gray_image);
		norm_gray_image = plate.PlateImage;

		if (gray_step == -1)
		{
			cv::Scalar mean = cv::sum(norm_gray_image) / norm_gray_image.cols / norm_gray_image.rows;
			min_gray_level = (int32_t)(mean[0] * 0.5);
			max_gray_level = (int32_t)(mean[0] * 1.6);
			gray_step = (max_gray_level - min_gray_level) / 20;
		}

		cv::Mat img_bw;

		SNFigureGroup group;

		for (int32_t gray_level = min_gray_level; gray_level <= max_gray_level; gray_level += gray_step)
		{
			img_bw = norm_gray_image > gray_level;

			ParseToFigures(img_bw, group);
		}

		sort(group.begin(), group.end(),
			[](const SNFigure & a, const SNFigure & b) -> bool
		{
			return a.left() < b.left();
		});

		RemoveEqualRects(group);

		cv::Mat out_image;

#ifdef IMAGE_DEBUG
		SNImageDebugger::DebugFigureGroup(plate.PlateImage, group, out_image, 4);
#endif // IMAGE_DEBUG

		RemoveBySize(plate.PlateImage, group);

#ifdef IMAGE_DEBUG
		out_image = cv::Mat(0, 0, CV_8UC1);
		SNImageDebugger::DebugFigureGroup(plate.PlateImage, group, out_image, 4);
#endif

		SNFigureGroups fgs;
		GroupByIntersect(group, plate.FigureGroups);

#ifdef IMAGE_DEBUG
		SNImageDebugger::DebugFigureGroups(plate.PlateImage, fgs, out_image, 2);
#endif
	}
	//---------------------------------------------------

	void SNSegmentor::RemoveEqualRects(SNFigureGroup& figs)
	{
		if (!figs.empty())
		for (SNFigureGroup::iterator i = figs.begin(); i != figs.end() - 1;)
		{
			SNFigureGroup::iterator next = i + 1;
			if (*i == *next)
				i = figs.erase(i);
			else
				++i;
		}
	}
	//------------------------------------------------------

	void SNSegmentor::ParseToFigures(cv::Mat& mat, SNFigureGroup& group)
	{
		group.reserve(1000);

		for (int nn = 0; nn < mat.rows; ++nn)
		{
			for (int mm = 0; mm < mat.cols; ++mm)
			{
				if (mat.at<unsigned char>(nn, mm) == 0)
				{
					SNFigure fig_to_create;
					AddPixelAsSpy(nn, mm, mat, fig_to_create, 1, mat.rows - 1);

					if (fig_to_create.is_too_big())
					{
						// проверяем что высота больше ширины
						if (fig_to_create.Width() < fig_to_create.Height() * 1.1)
						{
							if (fig_to_create.Width() > 4)
							{
								if (fig_to_create.Height() / fig_to_create.Width() < 4)
								{
									group.push_back(fig_to_create);
								}
							}
						}
					}
				}
			}
		}
	}
	//---------------------------------------------------

	void SNSegmentor::AddPixelAsSpy(int row, int col, cv::Mat& mat, SNFigure& fig, int top_border, int bottom_border)
	{
		//	time_mesure tm( "NEXT:" );
		if (top_border == -1) // верняя граница поиска
		{
			top_border = 0;
		}

		if (bottom_border == -1) // нижняя граница поиска
		{
			bottom_border = mat.rows;
		}

		PointsCount = 0;
		PointsDiplicateFirst[PointsCount] = row;
		PointsDiplicateSecond[PointsCount++] = col;

		// что бы не зациклилось
		mat.ptr<unsigned char>(row)[col] = 255;
		int cur_index = 0;
		while (cur_index < PointsCount)
		{
			const int& cur_nn = PointsDiplicateFirst[cur_index];
			const int& cur_mm = PointsDiplicateSecond[cur_index];

			for (size_t yy = 0; yy < sizeof(PixAround) / sizeof(PixAround[0]); ++yy)
			{
				const int &curr_pix_first = PixAround[yy].first + cur_nn;
				const int &curr_pix_second = PixAround[yy].second + cur_mm;
				if (curr_pix_first >= top_border && curr_pix_first < bottom_border
					&& curr_pix_second >= 0 && curr_pix_second < mat.cols)
				{
					if (mat.ptr< unsigned char >(curr_pix_first)[curr_pix_second] == 0)
					{
						PointsDiplicateFirst[PointsCount] = curr_pix_first;
						PointsDiplicateSecond[PointsCount++] = curr_pix_second;
						fig.AddPoint(pair_int(curr_pix_first, curr_pix_second));
						// что бы не зациклилось
						mat.ptr<unsigned char>(curr_pix_first)[curr_pix_second] = 255;
					}
				}
			}
			++cur_index;
		}

		//assert(fig.is_empty() || fig.left() >= 0);
	}
	//------------------------------------------------------

	void SNSegmentor::GroupByIntersect(const SNFigureGroup& figs, SNFigureGroups& groups)
	{
		for (auto f : figs)
		{
			if (groups.empty())
			{
				SNFigureGroup new_group;
				groups.push_back(new_group);
			}

			if (groups.back().empty())
			{
				groups.back().push_back(f);
			}
			else
			{
				cv::Rect r1 = cv::Rect(groups.back().front().left(), groups.back().front().top(), groups.back().front().Width(), groups.back().front().Height());
				cv::Rect r2 = cv::Rect(f.left(), f.top(), f.Width(), f.Height());
				cv::Rect intersect_rect = r1 & r2;
				if (intersect_rect.area() > 0.5 * r2.area() || intersect_rect.area() > 0.5 * r1.area())
				{
					groups.back().push_back(f);
				}
				else
				{
					SNFigureGroup new_group;
					groups.push_back(new_group);
					groups.back().push_back(f);
				}
			}
		}

		//KeepLargestRectOnly(groups);
	}
	//------------------------------------------------------

	void SNSegmentor::KeepLargestRectOnly(SNFigureGroups& fgs)
	{
		for (auto& fg : fgs)
		{
			SNFigure largest_figure;

			for (auto& f : fg)
			{
				if (f.Width() * f.Height() > largest_figure.Width() * largest_figure.Height())
				{
					largest_figure = f;
				}
			}

			fg.clear();
			fg.push_back(largest_figure);
		}
	}
	//------------------------------------------------------

	void SNSegmentor::RemoveBySize(const cv::Mat& plate_image, SNFigureGroup& figs)
	{
		float min_rel_height = 0.20f;
		float max_rel_height = 0.5f;

		//float min_rel_width = 0.01f;
		//float max_rel_width = 0.1f;

		float min_ar = 0.9f;
		float max_ar = 2.0f;

		for (auto f = figs.begin(); f != figs.end();)
		{
			float rel_height = f->Height() * 1.0f / plate_image.rows;
			float rel_width = f->Width() * 1.0f / plate_image.cols;
			float ar = f->Height() * 1.0f / f->Width();

			if (rel_height < min_rel_height || rel_height > max_rel_height || /*rel_width < min_rel_width || rel_width > max_rel_width || */ ar < min_ar || ar > max_ar)
				f = figs.erase(f);
			else
				++f;
		}
	}
	//------------------------------------------------------
}
