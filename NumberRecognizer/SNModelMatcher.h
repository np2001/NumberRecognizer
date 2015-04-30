#ifndef SNModelMatcher_h__
#define SNModelMatcher_h__
//------------------------------------------------------
#include "opencv2\highgui\highgui.hpp"
#include "SNMasterRecognizerStructs.h"
//------------------------------------------------------
typedef std::vector<cv::Rect> SNPlateModel;

struct SNPlateModelCenterVector
{
	cv::Point2f Vector;
	float VectorLength;
};

typedef std::vector<SNPlateModelCenterVector> SNPlateModelCenterVectors;
//------------------------------------------------------
class SNModelMatcher
{
public:
	SNModelMatcher();
	~SNModelMatcher();
	void AddModelElement(const cv::Rect& rect);
	void BuildModel(const cv::Rect& r1, const cv::Rect& r2, SNPlateModel& out_model);
	void DebugModel(const cv::Mat& gray_image, SNPlateModel& out_model, int scale = 1);
	float MatchModel(const cv::Mat& gray_image, SNFigureGroups& fgs);
private:
	SNPlateModel Model;
	SNPlateModelCenterVectors CenterVectors;
private:
	void CalcCenterVectors();
	cv::Point2f CalcCenter(const cv::Rect& r1);
	void CalcCenterVector(const cv::Point2f& c1, const cv::Point2f& c2, SNPlateModelCenterVector& vec);
	

};
//------------------------------------------------------

#endif // SNModelMatcher_h__
