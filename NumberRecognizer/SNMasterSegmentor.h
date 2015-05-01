#ifndef SNMasterSegmentor_h__
#define SNMasterSegmentor_h__
//-----------------------------------------------------------------------
#include <opencv2\core\core.hpp>
#include <stdint.h>
#include "SNMasterRecognizerStructs.h"
#include <set>
//-----------------------------------------------------------------------

class SNMasterSegmentor
{
public:
	SNMasterSegmentor();
	~SNMasterSegmentor();
	void Segment(const cv::Mat& gray_image, SNFigureGroup& groups, int32_t min_gray_level = 0, int32_t max_gray_level = 255, int32_t gray_step = -1);
	void GroupByIntersect(const SNFigureGroup& figs, SNFigureGroups& groups);

private:
	void AddPixelAsSpy(int row, int col, cv::Mat& mat, SNFigure& fig, int top_border = -1, int bottom_border = -1);
	void ParseToFigures(cv::Mat& mat, SNFigureGroup& group);
	void RemoveEqualRects(SNFigureGroup& figs);
	void KeepLargestRectOnly(SNFigureGroups& fgs);
private:
	int PointsDiplicateFirst[1024 * 1024 * 10];
	int PointsDiplicateSecond[1024 * 1024 * 10];
	int PointsCount;
	pair_int PixAround[8];
};
//-----------------------------------------------------------------------
#endif // SNMasterSegmentor_h__
