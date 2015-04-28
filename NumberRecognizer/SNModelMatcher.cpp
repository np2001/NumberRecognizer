#include "SNModelMatcher.h"
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

	cv::Point c0 = cv::Point2f(Model[0].x + Model[0].width / 2.0f, Model[0].y + Model[0].height / 2.0f);

	for (auto p : Model)
	{
		cv::Point c = cv::Point2f(p.x + p.width / 2.0f, p.y + p.height / 2.0f);
		CenterVectors.push_back(cv::Point2f(c - c0));
	}
}
//------------------------------------------------------

cv::Point2f SNModelMatcher::CalCenterVector(const cv::Rect& r1, const cv::Rect& r2)
{
	return cv::Point2f(r2.x + r2.width / 2.0f, r2.y + r2.height / 2.0f) - cv::Point2f(r1.x + r1.width / 2.0f, r1.y + r1.height / 2.0f);
}
//------------------------------------------------------

void SNModelMatcher::BuildModel(const cv::Rect& r1, const cv::Rect& r2, SNPlateModel& out_model)
{
	cv::Point2f c = CalCenterVector(r1, r2);
	cv::Point2f c0 = CenterVectors[1];
	c.
}
//------------------------------------------------------