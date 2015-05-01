#include "SNModelMatcher.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\video.hpp>

#include <windows.h>
#include <stdint.h>
//------------------------------------------------------

SNModelMatcher::SNModelMatcher()
{
	AddModelElement(cv::Rect(0, 7, 9, 13));
	AddModelElement(cv::Rect(12, 4, 10, 16));
	AddModelElement(cv::Rect(23, 4, 10, 16));
	AddModelElement(cv::Rect(34, 4, 10, 16));
	AddModelElement(cv::Rect(46, 7, 9, 13));
	AddModelElement(cv::Rect(56, 7, 9, 13));
	AddModelElement(cv::Rect(71, 2, 7, 13));
	AddModelElement(cv::Rect(79, 2, 7, 13));
	CalcCenterVectors();
}
//------------------------------------------------------

SNModelMatcher::~SNModelMatcher()
{
}
//------------------------------------------------------

void SNModelMatcher::AddModelElement(const cv::Rect& rect)
{
	Model.push_back(rect);
}
//------------------------------------------------------

void SNModelMatcher::CalcCenterVectors()
{
	CenterVectors.clear();

	//cv::Point2f c0 = CalcCenter(Model[0]);

	for (auto p : Model)
	{
		SNPlateModelCenterVector cvec;
		
		cv::Point2f c = CalcCenter(p);
		cvec.Vector = c;

		//CalcCenterVector(c0, c, cvec);

		CenterVectors.push_back(cvec);
	}
}
//------------------------------------------------------

cv::Point2f SNModelMatcher::CalcCenter(const cv::Rect& r1)
{
	return cv::Point2f(r1.x + r1.width / 2.0f, r1.y + r1.height / 2.0f);
}
//------------------------------------------------------

void SNModelMatcher::BuildModel(const cv::Rect& r1, const cv::Rect& r2, SNPlateModel& out_model)
{
	cv::Point2f c1 = CalcCenter(r1);
	cv::Point2f c2 = CalcCenter(r2);
	
	SNPlateModelCenterVector& cvec0 = CenterVectors[1];

	SNPlateModelCenterVector cvec;

	CalcCenterVector(c1, c2, cvec);

	float scale_ratio = cvec.VectorLength / cvec0.VectorLength;

	//out_model.push_back(r1);

	for (int i = 0; i < Model.size(); ++i)
	{
		cv::Point2f scaled_cv = CenterVectors[i].Vector * scale_ratio;

		cv::Point2f center = c1 + scaled_cv;
		cv::Rect r(center.x - Model[i].width * scale_ratio / 2, center.y - Model[i].height * scale_ratio / 2, Model[i].width * scale_ratio, Model[i].height * scale_ratio);

		out_model.push_back(r);
	}
}
//------------------------------------------------------

void SNModelMatcher::CalcCenterVector(const cv::Point2f& c1, const cv::Point2f& c2, SNPlateModelCenterVector& vec)
{
	vec.Vector = c2 - c1;
	vec.VectorLength = sqrt(vec.Vector.x * vec.Vector.x + vec.Vector.y * vec.Vector.y);
}
//------------------------------------------------------

void SNModelMatcher::DebugModel(const cv::Mat& gray_image, SNPlateModel& out_model, float mean_compare_ratio, int scale /*= 1*/)
{
	cv::Mat out_image;
	cvtColor(gray_image, out_image, CV_GRAY2RGB);
	cv:resize(out_image, out_image, out_image.size() * scale);

	for (auto m : out_model)
	{
		cv::Scalar color = CV_RGB(255, 0, 0);
		//// отрисовываем найденные фигуры
		cv::rectangle(out_image, m, color, 1);
	}

	int r = 0;
}
//------------------------------------------------------

bool SNModelMatcher::MatchModel(const cv::Mat& gray_image, SNFigureGroups& fgs, SNPlateModel& best_model)
{
	bool res = false;

	int min_symbols = 6;

	int best_i = 0;
	int best_j = 0;
	int best_m = 0;
	int best_n = 0;
	float best_compare_ratio_sum = 0.0f;

	for (int i = 0; i <= (int)fgs.size() - min_symbols; ++i)
	{
		for (int j = 1; j <= 2; ++j)
		{
			//for (auto& f1 : fgs[i])
			for (int m = 0; m < fgs[i].size(); ++m)
			{
				cv::Rect r1 = cv::Rect(fgs[i][m].left(), fgs[i][m].top(), fgs[i][m].Width(), fgs[i][m].Height());

				//for (auto& f2 : fgs[i + j])
				for (int n = 0; n < fgs[i + j].size(); ++n)
				{
					cv::Rect r2 = cv::Rect(fgs[i + j][n].left(), fgs[i + j][n].top(), fgs[i + j][n].Width(), fgs[i + j][n].Height());

					SNPlateModel pm;
					BuildModel(r1, r2, pm);

					float compare_ratio_sum = 0.0f;
					int compare_ratio_count = 0;

					for (int k = 0; k < pm.size(); ++k)
					{
						if (i + k >= fgs.size())
							break;

						float best_compare_ratio = 0.0f;

						cv::Rect r3 = cv::Rect(pm[k].x, pm[k].y, pm[k].width, pm[k].height);

						for (int l = 0; l < fgs[i + k].size(); ++l)
						{
							SNFigure f4 = fgs[i + k][l];
							cv::Rect r4 = cv::Rect(f4.left(), f4.top(), f4.Width(), f4.Height());

							float compare_ratio = (r3 & r4).area() * 1.0 / (r3 | r4).area();
							if (compare_ratio > best_compare_ratio)
								best_compare_ratio = compare_ratio;
						}

						compare_ratio_sum += best_compare_ratio;
						compare_ratio_count++;
					}

					compare_ratio_sum /= compare_ratio_count;

					if (compare_ratio_sum > best_compare_ratio_sum)
					{
						best_i = i;
						best_j = j;
						best_m = m;
						best_n = n;

						best_compare_ratio_sum = compare_ratio_sum;
					}

					/*char c[100];
					sprintf_s(c, 100, "CR = %2.2f\r\n", compare_ratio_sum);
					OutputDebugStringA(c);

					DebugModel(gray_image, pm);*/
				}
			}
		}
	}

	if (best_j != 0)
	{
		res = true;
		cv::Rect r1 = cv::Rect(fgs[best_i][best_m].left(), fgs[best_i][best_m].top(), fgs[best_i][best_m].Width(), fgs[best_i][best_m].Height());
		cv::Rect r2 = cv::Rect(fgs[best_i + best_j][best_n].left(), fgs[best_i + best_j][best_n].top(), fgs[best_i + best_j][best_n].Width(), fgs[best_i + best_j][best_n].Height());

		SNPlateModel pm;
		BuildModel(r1, r2, pm);

		DebugModel(gray_image, pm, 0);

		best_model = pm;

		for (int k = 0; k < pm.size(); ++k)
		{
			if (best_i + k >= fgs.size())
				break;

			float best_compare_ratio = 0.0f;

			cv::Rect r3 = cv::Rect(pm[k].x, pm[k].y, pm[k].width, pm[k].height);

			for (int l = 0; l < fgs[best_i + k].size(); ++l)
			{
				SNFigure f4 = fgs[best_i + k][l];
				cv::Rect r4 = cv::Rect(f4.left(), f4.top(), f4.Width(), f4.Height());

				float compare_ratio = (r3 & r4).area() * 1.0 / (r3 | r4).area();
				fgs[best_i + k][l].SetModelMatchRatio(compare_ratio);
			}
		}
	}

	return res;
}
//------------------------------------------------------

cv::Point2f SNModelMatcher::AffineTransform(const cv::Point2f& p, const cv::Mat& trans_mat)
{
	cv::Mat x = cv::Mat(3, 1, CV_64FC1);
	x.at<double>(0) = p.x;
	x.at<double>(1) = p.y;
	x.at<double>(2) = 1;

	cv::Mat lt = trans_mat * x;

	return cv::Point2f(lt.at<double>(0), lt.at<double>(1));
}

void SNModelMatcher::MatchModel(const cv::Mat& gray_image, SNFigureGroup& fg)
{
	for (auto f : fg)
	{
		cv::Point2f src[3];
		cv::Point2f dst[3];
		/// Set your 3 points to calculate the  Affine Transform
		
		src[0] = cv::Point2f(Model[1].x, Model[1].y);
		src[1] = cv::Point2f(Model[1].x, Model[1].y + Model[1].height);
		src[2] = cv::Point2f(Model[1].x + Model[1].width, Model[1].y);


		dst[0] = cv::Point2f(f.left(), f.top());
		dst[1] = cv::Point2f(f.left(), f.top() + f.Height());
		dst[2] = cv::Point2f(f.left() + f.Width(), f.top());

		/// Get the Affine Transform
		cv::Mat warp_mat = getAffineTransform(src, dst);

		SNPlateModel pm;
		//pm.push_back(cv::Rect(f.left(), f.top(), f.Width(), f.Height()));

		for (int i = 0; i < Model.size(); ++i)
		{
			cv::Point2f p1 = AffineTransform(cv::Point2f(Model[i].x, Model[i].y), warp_mat);
			cv::Point2f p2 = AffineTransform(cv::Point2f(Model[i].x, Model[i].y + Model[i].height), warp_mat);
			cv::Point2f p3 = AffineTransform(cv::Point2f(Model[i].x + Model[i].width, Model[i].y), warp_mat);

			pm.push_back(cv::Rect(p1, cv::Size(p3.x - p1.x + 1, p2.y - p1.y + 1)));
		}

		DebugModel(gray_image, pm, 0);
	}
}
//------------------------------------------------------

bool SNModelMatcher::MatchModel2(const cv::Mat& gray_image, SNFigureGroups& fgs, int32_t& best_group_to_start)
{
	std::vector<cv::Point2f> src;

	for (int i = 0; i < (int)CenterVectors.size() - 2; ++i)
	{
		src.push_back(CenterVectors[i].Vector);
	}
	
	cv::Point2f dst[3];

	float best_compare_ratio = 0.0f;

	for (int i = 0; i <= (int)fgs.size() - (int)Model.size() + 2; ++i)
	{
		std::vector<cv::Point2f> dst;

		for (int j = 0; j < (int)Model.size() - 2; ++j)
		{
			dst.push_back(cv::Point2f(fgs[i + j].front().center().first, fgs[i + j].front().center().second));
		}

		cv::Mat trans_mat = cv::estimateRigidTransform(src, dst, false);

		SNPlateModel pm;

		float compare_ratio_sum = 0.0f;
		int compare_ratio_count = 0;

		for (int k = 0; k < Model.size(); ++k)
		{
			cv::Point2f tl = AffineTransform(cv::Point2f(Model[k].tl()), trans_mat);
			cv::Point2f br = AffineTransform(cv::Point2f(Model[k].br()), trans_mat);

			cv::Rect model_rect = cv::Rect(tl, br);

			if (i + k < (int)fgs.size())
			{
				cv::Rect detected_rect = cv::Rect(fgs[i + k].front().left(), fgs[i + k].front().top(), fgs[i + k].front().Width(), fgs[i + k].front().Height());

				float compare_ratio = (model_rect & detected_rect).area() * 1.0 / (model_rect | detected_rect).area();
				compare_ratio_sum += compare_ratio;
				compare_ratio_count++;
			}

			pm.push_back(model_rect);
		}

		compare_ratio_sum /= compare_ratio_count;

		if (compare_ratio_sum > best_compare_ratio)
		{
			best_compare_ratio = compare_ratio_sum;
			best_group_to_start = i;
		}

		//DebugModel(gray_image, pm, compare_ratio_sum);
	}

	return true;
}
//------------------------------------------------------