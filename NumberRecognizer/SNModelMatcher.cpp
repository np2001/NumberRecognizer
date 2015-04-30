#include "SNModelMatcher.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <windows.h>
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

	cv::Point2f c0 = CalcCenter(Model[0]);

	for (auto p : Model)
	{
		SNPlateModelCenterVector cvec;
		
		cv::Point2f c = CalcCenter(p);

		CalcCenterVector(c0, c, cvec);

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

void SNModelMatcher::DebugModel(const cv::Mat& gray_image, SNPlateModel& out_model, int scale /*= 1*/)
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

float SNModelMatcher::MatchModel(const cv::Mat& gray_image, SNFigureGroups& fgs, SNPlateModel& best_model)
{
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

	{
		cv::Rect r1 = cv::Rect(fgs[best_i][best_m].left(), fgs[best_i][best_m].top(), fgs[best_i][best_m].Width(), fgs[best_i][best_m].Height());
		cv::Rect r2 = cv::Rect(fgs[best_i + best_j][best_n].left(), fgs[best_i + best_j][best_n].top(), fgs[best_i + best_j][best_n].Width(), fgs[best_i + best_j][best_n].Height());

		SNPlateModel pm;
		BuildModel(r1, r2, pm);

		DebugModel(gray_image, pm);

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

	return 0.0;
}
//------------------------------------------------------