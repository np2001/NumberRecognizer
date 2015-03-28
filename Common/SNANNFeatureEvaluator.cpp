#include "SNANNFeatureEvaluator.h"
#include "opencv2\imgproc\imgproc.hpp"
//---------------------------------------------------------------------

SNANNFeatureEvaluator::SNANNFeatureEvaluator()
{
}
//---------------------------------------------------------------------

SNANNFeatureEvaluator::~SNANNFeatureEvaluator()
{
}
//---------------------------------------------------------------------

//cv::Mat SNANNFeatureEvaluator::Features(const cv::Mat& gray_image)
//{
//	cv::Mat low_data;
//
//	cv::resize(gray_image, low_data, cv::Size(8, 16));
//
//	cv::threshold(low_data, low_data, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
//	
//	int num_cols = low_data.cols * low_data.rows;
//
//	cv::Mat out = cv::Mat::zeros(1, num_cols, CV_32F);
//
//	int j = 0;
//
//	for (int x = 0; x < low_data.cols; x++)
//	{
//		for (int y = 0; y < low_data.rows; y++)
//		{
//			out.at<float>(j) = (float)low_data.at<unsigned char>(y, x);
//			j++;
//		}
//	}
//
//	return out;
//}
////-------------------------------------------------------------------------


cv::Mat SNANNFeatureEvaluator::Features(const cv::Mat& gray_image)
{
	cv::Mat low_data;

	cv::resize(gray_image, low_data, cv::Size(8, 16));

	//cv::threshold(low_data, low_data, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	//cv::equalizeHist(low_data, low_data);

	cv::Mat out = cv::Mat::zeros(1, 256, CV_32F);

	for (int x = 0; x < low_data.cols; x++)
	{
		for (int y = 0; y < low_data.rows; y++)
		{
			unsigned char center_value = (float)low_data.at<unsigned char>(y, x);

			uint8_t value = 0;

			LBPBit(low_data, x - 1, y - 1, center_value, 1, value);
			LBPBit(low_data, x, y - 1, center_value, 2, value);
			LBPBit(low_data, x + 1, y - 1, center_value, 4, value);
			LBPBit(low_data, x - 1, y, center_value, 8, value);
			LBPBit(low_data, x + 1, y, center_value, 16, value);
			LBPBit(low_data, x - 1, y + 1, center_value, 32, value);
			LBPBit(low_data, x, y + 1, center_value, 64, value);
			LBPBit(low_data, x + 1, y + 1, center_value, 128, value);

			out.at<float>(value)++;

		}
	}

	return out;
}
//------------------------------------------------------------------------ -

void SNANNFeatureEvaluator::LBPBit(const cv::Mat& low_data, int32_t x, int32_t y, const uint8_t& center_value, const uint8_t& bitmask, uint8_t& value)
{
	if (x >= 0 && x < low_data.cols && y >= 0 && y < low_data.rows)
	{
		unsigned char v = (float)low_data.at<unsigned char>(y, x);
		if (v > center_value)
		{
			value |= bitmask;
		}
	}
}
//-------------------------------------------------------------------------
