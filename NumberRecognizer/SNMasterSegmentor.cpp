#include "SNMasterSegmentor.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <set>
//---------------------------------------------------

SNMasterSegmentor::SNMasterSegmentor()
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

SNMasterSegmentor::~SNMasterSegmentor()
{
}
//---------------------------------------------------

void SNMasterSegmentor::Segment(const cv::Mat& gray_image, SNFigureGroup& group, int32_t min_gray_level /*= 0*/, int32_t max_gray_level /*= 255*/, int32_t gray_step /*= -1*/)
{
	cv::Mat norm_gray_image;

	cv::equalizeHist(gray_image, norm_gray_image);
	
	if (gray_step == -1)
	{
		cv::Scalar mean = cv::sum(norm_gray_image) / norm_gray_image.cols / norm_gray_image.rows;
 		min_gray_level = mean[0] * 0.5;
		max_gray_level = mean[0] * 1.6;
		gray_step = (max_gray_level - min_gray_level) / 20;
	}
	
	cv::Mat img_bw;

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
}
//---------------------------------------------------

void SNMasterSegmentor::RemoveEqualRects(SNFigureGroup& figs)
{
	if (!figs.empty())
	for (SNFigureGroup::iterator i = figs.begin(); i != figs.end() - 1; )
	{
		SNFigureGroup::iterator next = i + 1;
		if (*i == *next)
			i = figs.erase(i);
		else
			++i;
	}
}
//------------------------------------------------------

void SNMasterSegmentor::ParseToFigures(cv::Mat& mat, SNFigureGroup& group)
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

void SNMasterSegmentor::AddPixelAsSpy(int row, int col, cv::Mat& mat, SNFigure& fig, int top_border, int bottom_border)
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
					mat.ptr< unsigned char >(curr_pix_first)[curr_pix_second] = 255;
				}
			}
		}
		++cur_index;
	}

	//assert(fig.is_empty() || fig.left() >= 0);
}
//------------------------------------------------------

void SNMasterSegmentor::GroupByIntersect(const SNFigureGroup& figs, SNFigureGroups& groups)
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

	KeepLargestRectOnly(groups);
}
//------------------------------------------------------

void SNMasterSegmentor::KeepLargestRectOnly(SNFigureGroups& fgs)
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


