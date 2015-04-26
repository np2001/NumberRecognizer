#ifndef SNMasterRecognizer_h__
#define SNMasterRecognizer_h__
//------------------------------------------------------
#include <vector>
#include <stdint.h>
#include <opencv2\core\core.hpp>
#include <set>
#include "SNMasterRecognizerStructs.h"
//------------------------------------------------------
typedef std::vector<int> SNSearchLevels;
typedef std::pair<double, double> pair_doub;
#define COUNT_GLOBAL_DATA 8
//------------------------------------------------------

inline pair_int operator - (const pair_int& lh, const pair_int& rh)
{
	return std::make_pair(lh.first - rh.first, lh.second - rh.second);
}

inline pair_int operator + (const pair_int& lh, const pair_int& rh)
{
	return std::make_pair(lh.first + rh.first, lh.second + rh.second);
}

inline pair_int operator * (const pair_int& lh, const double& koef)
{
	return std::make_pair(static_cast<int>(static_cast<double>(lh.first) * koef), static_cast<int>(static_cast<double>(lh.second) * koef));
}

inline pair_int operator / (const pair_int& lh, const int& koef)
{
	return std::make_pair(lh.first / koef, lh.second / koef);
}

inline pair_doub operator * (const pair_doub& lh, const double& koef)
{
	return std::make_pair(static_cast<double>(lh.first * koef), static_cast<double>(lh.second * koef));
}
namespace cv
{
	inline bool operator < (const cv::Rect& lh, const cv::Rect& rh)
	{
		return memcmp(&lh, &rh, sizeof(cv::Rect)) < 0;
	}
}


class SNMasterRecognizer
{
public:
	SNMasterRecognizer();
	~SNMasterRecognizer();
	SNFoundNumber ReadNumberLoop(const cv::Mat& input);
private:
	SNSearchLevels SearchLevels;
	bool BusyIndexes[COUNT_GLOBAL_DATA];

	int PointsDiplicateFirst[COUNT_GLOBAL_DATA][1024 * 1024 * 10];
	int PointsDiplicateSecond[COUNT_GLOBAL_DATA][1024 * 1024 * 10];
	int PointsCount[COUNT_GLOBAL_DATA];
	pair_int PixAround[COUNT_GLOBAL_DATA][4] ;
	std::set<std::string> RegionCodes;

private:
	void SetSearchLevels(int32_t gray_step = 10);
	int GetFreeIndex();
	std::vector<SNFoundNumber> ReadNumberImpl(int stat_data_index, const cv::Mat& input, int gray_level, SNNumberData& stat_data);
	std::vector<SNFigure> ParseToFigures(int stats_data_index, cv::Mat& mat);
	void AddPixelAsSpy(int stat_data_index, int row, int col, cv::Mat& mat, SNFigure& fig, int top_border = -1, int bottom_border = -1);
	std::vector<SNFigureGroup> MakeGroups(std::vector<SNFigure>& figs);
	bool AngleIsEqual(int an1, int an2);
	friend bool FlgLessLeft(const SNFigure& lf, const SNFigure& rf);
	void FigsRemoveTooLongByXFromFirst(std::vector<SNFigureGroup>& groups);
	void GroupsRemoveTooSmall(std::vector<SNFigureGroup>& groups);
	void FigsRemoveTooLongByYFromFirst(std::vector<SNFigureGroup>& groups);
	void FigsRemoveInvalidFromFirstBySize(std::vector<SNFigureGroup> & groups);
	void GroupsRemoveIncluded(std::vector<SNFigureGroup> & groups);
	void GroupsMergeIntersects(std::vector<SNFigureGroup> & groups);
	std::vector<SNFoundNumber> SearchNumber(cv::Mat& etal, std::vector<SNFigureGroup>& groups, const cv::Mat& original, SNNumberData& stat_data);
	std::vector<pair_int> CalcSymCenters(int index, int angle, int first_fig_height);
	std::vector<SNFoundSymbol> FigsSearchSyms(const std::vector<pair_int>& pis, const pair_int& pos_center, const SNFigureGroup& cur_gr, const cv::Mat& original, SNNumberData& stat_data);
	std::pair<char, double> FindSymNN(bool num, const SNFigure& fig, const cv::Mat& original, SNNumberData& stat_data);
	std::pair<char, double> ProcChar(const cv::Mat& input);
	std::pair<char, double> ProcNum(const cv::Mat& input);
	SNFoundNumber CreateNumberByPos(const std::vector<pair_int>& pis, const std::vector<SNFoundSymbol>& figs_by_pos);
	SNFoundNumber FindBestNumberByWeight(const std::vector<SNFoundNumber>& vals, const cv::Mat* etal = 0);
	int FindBestIndex(const std::vector<SNFoundNumber>& found_nums);
	void SetFreeIndex(int index);
	void SearchRegion(int stats_data_index, SNFoundNumber& best_number, const int best_level, const cv::Mat& original, SNNumberData& stat_data);
	friend bool NotInCharDistance(int val);
	std::vector< std::vector< pair_doub > > Get2SymRegKoef(const std::vector<SNFigure>& figs, double avarage_height, double sin_avarage_angle_by_y);
	void FillReg(std::vector<std::vector<pair_doub> >& data, double x1, double y1, double x2, double y2, double x3, double y3);
	void FillReg(std::vector<std::vector<pair_doub>>& data, double x1, double y1, double x2, double y2);
	void ApplyAngle(const std::vector<SNFigure>& figs, std::vector<std::vector< pair_doub > >& data, double avarage_height, double sin_avarage_angle_by_y);
	void SearchRegionSymbol(int stat_data_index, SNFoundNumber& number, const cv::Mat& etal, const cv::Mat& origin, const pair_int& reg_center, const double avarage_height, bool last_symbol, SNNumberData& stat_data);
	pair_int SearchNearestBlack(const cv::Mat& etal, const pair_int& center);
	pair_int CalcCenter(const std::vector<SNFigure>& figs, const std::vector< std::vector< pair_doub > >& data, int index);
	std::vector<std::vector< pair_doub > > Get3SymRegKoef(const std::vector<SNFigure>& figs, double avarage_height, double sin_avarage_angle_by_y);
	pair_int GetPosNextInRegion(const std::vector<SNFigure>& figs, const std::vector< std::vector< pair_doub > >& move_reg, const int index, const SNFoundNumber& number, const double avarage_height);
	bool CompareRegions(const SNFoundNumber& lh, const SNFoundNumber& rh);
};

//template< class T > std::set< T > to_set(const std::vector<T>& in);
//------------------------------------------------------
#endif // SNMasterRecognizer_h__
