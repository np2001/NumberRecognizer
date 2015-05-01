#ifndef SNModelMatcher_h__
#define SNModelMatcher_h__
//------------------------------------------------------
#include "opencv2\highgui\highgui.hpp"
#include "SNMasterRecognizerStructs.h"
#include <stdint.h>
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
	void DebugModel(const cv::Mat& gray_image, SNPlateModel& out_model, float mean_compare_ratio, int scale = 1);
	bool MatchModel(const cv::Mat& gray_image, SNFigureGroups& fgs, SNPlateModel& best_model);
	bool MatchModel2(const cv::Mat& gray_image, SNFigureGroups& fgs, int32_t& best_group_to_start);

	void MatchModel(const cv::Mat& gray_image, SNFigureGroup& fg);
private:
	SNPlateModel Model;
	SNPlateModelCenterVectors CenterVectors;
private:
	void CalcCenterVectors();
	cv::Point2f CalcCenter(const cv::Rect& r1);
	void CalcCenterVector(const cv::Point2f& c1, const cv::Point2f& c2, SNPlateModelCenterVector& vec);
	cv::Point2f AffineTransform(const cv::Point2f& p, const cv::Mat& trans_mat);


};
//------------------------------------------------------

#endif // SNModelMatcher_h__
