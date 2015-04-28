#ifndef SNModelMatcher_h__
#define SNModelMatcher_h__
//------------------------------------------------------
#include "opencv2\highgui\highgui.hpp"
//------------------------------------------------------
typedef std::vector<cv::Rect> SNPlateModel;
typedef std::vector<cv::Point2f> SNPlateModelCenterVectors;
//------------------------------------------------------
class SNModelMatcher
{
public:
	SNModelMatcher();
	~SNModelMatcher();
	void AddModelElement(const cv::Rect& rect);
	void BuildModel(const cv::Rect& r1, const cv::Rect& r2, SNPlateModel& out_model);
private:
	SNPlateModel Model;
	SNPlateModelCenterVectors CenterVectors;
private:
	void CalcCenterVectors();
	cv::Point2f CalCenterVector(const cv::Rect& r1, const cv::Rect& r2);
};
//------------------------------------------------------

#endif // SNModelMatcher_h__
