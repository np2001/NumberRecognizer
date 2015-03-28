#include "SNHistogramSegmentor.h"
#include "opencv2\imgproc\imgproc.hpp"
#include <stdint.h>
//-------------------------------------------------------------------------------
#define NORM_IMAGE_SIZE cv::Size(268, 60)
//-------------------------------------------------------------------------------

namespace SNNumberRecognizer
{
	SNHistogramSegmentor::SNHistogramSegmentor()
	{
	}
	//-------------------------------------------------------------------------------

	SNHistogramSegmentor::~SNHistogramSegmentor()
	{
	}
	//-------------------------------------------------------------------------------

	void SNHistogramSegmentor::LoadImage(const cv::Mat& gray_image)
	{
		cv::Mat tmp_image;
		cv::resize(gray_image, tmp_image, NORM_IMAGE_SIZE);
	
		cv::integral(tmp_image, IntegralImage, CV_32S);
	}
	//-------------------------------------------------------------------------------

	void SNHistogramSegmentor::DetectPeaks(const cv::Mat& v_hist, SNSymbolRanges& symbol_ranges)
	{
		int step = 3;

		for (int k = step; k < v_hist.cols - step; k += step)
		{
			float dx1 = v_hist.at<float>(k - step) - v_hist.at<float>(k);
			float dx2 = v_hist.at<float>(k + step) - v_hist.at<float>(k);

			float dx = dx1 * dx2;
			int sign = dx1 > 0 ? 1 : dx1 < 0 ? -1 : 0;

			if (sign > 0)
			{
				if (!symbol_ranges.size() || symbol_ranges.back().EndMax != -1)
				{
					SNSymbolRange new_range;
					new_range.StartMin = k;
					new_range.StartMax = k;
					symbol_ranges.push_back(new_range);
				}
				else
				{
					symbol_ranges.back().StartMax = k;
				}
			}
			else
			if (sign < 0)
			{
				if (symbol_ranges.size())
				{
					if (symbol_ranges.back().EndMin == -1)
						symbol_ranges.back().EndMin = k;

					symbol_ranges.back().EndMax = k;
				}
			}
		}

		if (symbol_ranges.size() && symbol_ranges.back().EndMin == -1)
		{
			symbol_ranges.pop_back();
		}
	}
	//-------------------------------------------------------------------------------

	void SNHistogramSegmentor::CalcVHist(cv::Mat& v_hist)
	{
		v_hist = cv::Mat::zeros(1, IntegralImage.cols - 1, CV_32F);

		for (int j = 0; j < v_hist.cols; j++)
		{
			float sum = 0;

			float a = IntegralImage.at<int32_t>(0, j);
			float b = IntegralImage.at<int32_t>(IntegralImage.rows - 1, j + 1);
			float c = IntegralImage.at<int32_t>(0, j + 1);
			float d = IntegralImage.at<int32_t>(IntegralImage.rows - 1, j);

			sum = a + b - c - d;

			if (sum < 0)
			{
				int r = 0;
			}

			v_hist.at<float>(j) = sum;
		}

		//Normalize histogram
		double min, max;
		minMaxLoc(v_hist, &min, &max);
		
		if (max > 0)
			v_hist.convertTo(v_hist, -1, 1.0f / max, 0);
	}
	//-------------------------------------------------------------------------------

	cv::Mat SNHistogramSegmentor::DebugVHist(const cv::Mat& v_hist, const cv::Mat& rgb_image)
	{
		cv::Mat color_image;
		cv::resize(rgb_image, color_image, NORM_IMAGE_SIZE);

		for (uint32_t i = 0; i < v_hist.cols; ++i)
		{
			cv::line(color_image, cv::Point(i, 0), cv::Point(i, v_hist.at<uint8_t>(i) * color_image.rows / 300.0), cv::Scalar(0, 0, 255));
		}

		return color_image;
	}
	//-------------------------------------------------------------------------------

	void SNHistogramSegmentor::DetectSymbolRanges(SNSymbolRanges& sr)
	{
		cv::Mat v_hist;
		CalcVHist(v_hist);
		DetectPeaks(v_hist, sr);
	}
	//-------------------------------------------------------------------------------

	void SNHistogramSegmentor::DebugSymbolRanges(SNSymbolRanges& sr, cv::Mat& rgb_image)
	{
		cv::Mat color_image;

		cv::resize(rgb_image, color_image, NORM_IMAGE_SIZE);

		for (auto r : sr)
		{
			cv::rectangle(color_image, cv::Rect(r.StartMin, 0, r.EndMax - r.StartMin, color_image.rows), cv::Scalar(255, 0, 0));
		}

		int r = 0;
	}
	//-------------------------------------------------------------------------------
}
//-------------------------------------------------------------------------------
