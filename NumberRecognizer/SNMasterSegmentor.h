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
	void Segment(const cv::Mat& gray_image, SNFigureGroups& groups, int32_t min_gray_level = 0, int32_t max_gray_level = 255, int32_t gray_step = -1);
	void DebugFigureGroups(const cv::Mat& gray_image, const SNFigureGroups& groups, cv::Mat& out_image, int scale = 1);

private:
	void AddPixelAsSpy(int row, int col, cv::Mat& mat, SNFigure& fig, int top_border = -1, int bottom_border = -1);
	void ParseToFigures(cv::Mat& mat, SNFigureGroup& group);
	void MakeGroups(const SNFigureGroup& figs, SNFigureGroups& groups);
	bool AngleIsEqual(int an1, int an2);
	void RemoveEqualRects(SNFigureGroup& figs);
	void GroupByX(const SNFigureGroup& figs, SNFigureGroups& groups, int32_t group_step);
	void GroupByIntersect(const SNFigureGroup& figs, SNFigureGroups& groups);

	void FigsRemoveTooLongByXFromFirst(SNFigureGroups& groups);
	void GroupsRemoveTooSmall(SNFigureGroups& groups);
	void FigsRemoveTooLongByYFromFirst(SNFigureGroups& groups);
	void FigsRemoveInvalidFromFirstBySize(SNFigureGroups& groups);
	void GroupsMergeIntersects(SNFigureGroups& groups);
	void GroupsRemoveIncluded(SNFigureGroups& groups);
	std::set<SNFigure> ToSet(const SNFigureGroup& in);
	std::vector<SNFoundNumber> SearchNumber(cv::Mat& etal, std::vector<SNFigureGroup>& groups, const cv::Mat& original, SNNumberData& stat_data);
private:
	int PointsDiplicateFirst[1024 * 1024 * 10];
	int PointsDiplicateSecond[1024 * 1024 * 10];
	int PointsCount;
	pair_int PixAround[4];
};
//-----------------------------------------------------------------------
#endif // SNMasterSegmentor_h__
