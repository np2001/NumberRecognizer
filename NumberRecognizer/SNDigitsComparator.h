#ifndef SNDigitsComparator_h__
#define SNDigitsComparator_h__
//----------------------------------------------------------------------
#include <string>
#include "opencv2\highgui\highgui.hpp"
#include <opencv2\core\core.hpp>
#include <stdint.h>
//----------------------------------------------------------------------
typedef std::map<char, cv::Mat> SNDigitsComparatorTemplates;
typedef std::map<char, int32_t> SNDigitsComparatorResults;
//----------------------------------------------------------------------

class SNDigitsComparator
{
public:
	SNDigitsComparator();
	~SNDigitsComparator();
	void LoadDigitsTemplate(std::string filename);
	void Compare(const cv::Mat& image, SNDigitsComparatorResults& results);
	void CompareLBP(const cv::Mat& image, SNDigitsComparatorResults& results);
private:
	SNDigitsComparatorTemplates Templates;
	cv::Size WindowSize;
private:
	void AddTemplate(const char& id, const cv::Mat& template_image, cv::Point2i& top_left);
	int32_t Compare(const cv::Mat& m1, const cv::Mat& m2);
	void CalcLBP(const cv::Mat& m1, cv::Mat& lbp);
	bool LBPBit(const cv::Mat& low_data, int32_t x, int32_t y, const uint8_t& center_value);
	int32_t CompareLBP(const cv::Mat& m1, const cv::Mat& m2);
	
};
//----------------------------------------------------------------------
#endif // SNDigitsComparator_h__
