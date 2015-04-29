#include "SNModelMatcher.h"
#include <opencv2\imgproc\imgproc.hpp>
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

float SNModelMatcher::MatchModel(const cv::Mat& gray_image, const SNFigureGroups fgs)
{
	int min_symbols = 6;

	for (int i = 0; i < fgs.size() - min_symbols; ++i)
	{
		for (int j = 1; j <= 2; ++j)
		{
			for (auto& f1 : fgs[i])
			{
				cv::Rect r1 = cv::Rect(f1.left(), f1.top(), f1.Width(), f1.Height());

				for (auto& f2 : fgs[i + j])
				{
					cv::Rect r2 = cv::Rect(f2.left(), f2.top(), f2.Width(), f2.Height());

					SNPlateModel pm;
					BuildModel(r1, r2, pm);
					//DebugModel(gray_image, pm);
				}
			}
		}
	}

	return 0.0;
}
//------------------------------------------------------