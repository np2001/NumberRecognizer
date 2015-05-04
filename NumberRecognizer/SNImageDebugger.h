#ifndef SNImageDebugger_h__
#define SNImageDebugger_h__
//--------------------------------------------------------------------------------------
#include "opencv2\core\core.hpp"
#include "SNMasterRecognizerStructs.h"
//--------------------------------------------------------------------------------------

class SNImageDebugger
{
public:
	SNImageDebugger();
	~SNImageDebugger();
	static void DebugFigureGroup(const cv::Mat& gray_image, const SNFigureGroup& group, cv::Mat& out_image, int scale = 1, cv::Scalar color = cvScalar(255, 0, 0));
	static void DebugFigureGroups(const cv::Mat& gray_image, const SNFigureGroups& groups, cv::Mat& out_image, int scale = 1);
};
//--------------------------------------------------------------------------------------
#endif // SNImageDebugger_h__
