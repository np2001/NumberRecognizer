#ifndef SNHistogramSegmentor_h__
#define SNHistogramSegmentor_h__
//-------------------------------------------------------------------------------
#include "opencv2\core\core.hpp"
#include <list>
#include <stdint.h>
//-------------------------------------------------------------------------------
namespace SNNumberRecognizer
{
	struct SNSymbolRange;

	typedef std::vector<SNSymbolRange> SNSymbolRanges;
	typedef std::list<SNSymbolRanges> SNSymbolRangesList;

	struct SNSymbolRange
	{
		int32_t StartMin;
		int32_t StartMax;
		int32_t EndMin;
		int32_t EndMax;

		cv::Mat VHist;

		SNSymbolRanges VRanges;

		SNSymbolRange()
		{
			StartMin = -1;
			StartMax = -1;
			EndMin = -1;
			EndMax = -1;
		}
	};

	
	class SNHistogramSegmentor
	{
	public:
		SNHistogramSegmentor();
		~SNHistogramSegmentor();
		void LoadImage(const cv::Mat& gray_image);
		void CalcHHistogram(cv::Mat& v_hist, SNSymbolRanges& symbol_ranges);
		cv::Mat ProjectedHistogram(cv::Mat integral_image, int t);
		void DetectPeaks(const cv::Mat& v_hist, SNSymbolRanges& symbol_ranges);
		void DebugHHist(cv::Mat image, cv::Mat& v_hist, const SNSymbolRanges& symbol_ranges, cv::Mat& out_image);
		void CombineSegments(const SNSymbolRanges& symbol_ranges, SNSymbolRangesList& plate_symbol_ranges);
		void DebugCombinedSegments(cv::Mat image, const SNSymbolRangesList& plate_symbol_ranges, cv::Mat out_image);
		void CalcVHist(SNSymbolRanges& symbol_ranges);
		void CalcVHist(cv::Mat& v_hist);
		cv::Mat DebugVHist(const cv::Mat& v_hist, const cv::Mat& color_image);
		void DetectSymbolRanges(SNSymbolRanges& sr);
		void DebugSymbolRanges(SNSymbolRanges& sr, cv::Mat& color_image);

	private:
		cv::Mat IntegralImage;
	};
}
//-------------------------------------------------------------------------------

#endif // SNHistogramSegmentor_h__

