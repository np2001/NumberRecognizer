#include "SNDigitsComparator.h"
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <stdint.h>
#include <opencv2\imgproc\imgproc.hpp>
//----------------------------------------------------------------------

SNDigitsComparator::SNDigitsComparator()
{
	WindowSize = cv::Size(10, 16);
}
//----------------------------------------------------------------------

SNDigitsComparator::~SNDigitsComparator()
{
}
//----------------------------------------------------------------------

void SNDigitsComparator::LoadDigitsTemplate(std::string filename)
{
	cv::Mat template_image = cv::imread(filename, cv::IMREAD_GRAYSCALE);
	AddTemplate('0', template_image, cv::Point2i(0, 0));
	AddTemplate('1', template_image, cv::Point2i(19, 0));
	AddTemplate('2', template_image, cv::Point2i(39, 0));
	AddTemplate('3', template_image, cv::Point2i(59, 0));
	AddTemplate('4', template_image, cv::Point2i(78, 0));
	AddTemplate('5', template_image, cv::Point2i(93, 0));
	AddTemplate('6', template_image, cv::Point2i(113, 0));
	AddTemplate('7', template_image, cv::Point2i(132, 0));
	AddTemplate('8', template_image, cv::Point2i(152, 0));
	AddTemplate('9', template_image, cv::Point2i(172, 0));
}
//----------------------------------------------------------------------

void SNDigitsComparator::AddTemplate(const char& id, const cv::Mat& template_image, cv::Point2i& top_left)
{
	Templates[id] = cv::Mat(template_image, cv::Rect(top_left.x, top_left.y, 10, 16));
	cv::resize(Templates[id], Templates[id], WindowSize);
}
//----------------------------------------------------------------------

int32_t SNDigitsComparator::Compare(const cv::Mat& m1, const cv::Mat& m2)
{
	int32_t diff = 0;
	for (int i = 0; i < WindowSize.height; ++i)
	{
		for (int j = 0; j < WindowSize.width; ++j)
		{
			uint8_t b1 = m1.at<uint8_t>(i, j);
			uint8_t b2 = m2.at<uint8_t>(i, j);
			if (b1 < b2)
			{
				diff += (b2 - b1);
			}
			else
			{
				diff += (b1 - b2);
			}
		}
	}

	return diff;
}
//----------------------------------------------------------------------

void SNDigitsComparator::Compare(const cv::Mat& image, SNDigitsComparatorResults& results)
{
	cv::Mat resized_image;

	cvtColor(image, resized_image, CV_RGB2GRAY);
	cv::resize(resized_image, resized_image, WindowSize);
	cv::threshold(resized_image, resized_image, 128, 255, CV_THRESH_OTSU);

	int32_t sum = 0;
	for (auto& r : Templates)
	{
		int32_t res = Compare(resized_image, r.second);
		results[r.first] = res;

		sum += res;
	}

	for (auto& r : results)
	{
		r.second = r.second * 100 / sum;
	}
}
//----------------------------------------------------------------------

void SNDigitsComparator::CalcLBP(const cv::Mat& m1, cv::Mat& lbp)
{
	lbp = cv::Mat::zeros(1, m1.cols * m1.rows * 4, CV_8UC1);
	int index = 0;
	for (int i = 0; i < m1.rows; ++i)
	{
		for (int j = 0; j < m1.cols; ++j)
		{
			uint8_t v = m1.at<uint8_t>(i, j);
			lbp.at<uint8_t>(index++) = LBPBit(m1, j - 1, i + 1, v) ? 255 : 0;
			lbp.at<uint8_t>(index++) = LBPBit(m1, j, i + 1, v) ? 255 : 0;
			lbp.at<uint8_t>(index++) = LBPBit(m1, j + 1, i + 1, v) ? 255 : 0;
			lbp.at<uint8_t>(index++) = LBPBit(m1, j + 1, i, v) ? 255 : 0;
		}
	}
}
//----------------------------------------------------------------------

bool SNDigitsComparator::LBPBit(const cv::Mat& low_data, int32_t x, int32_t y, const uint8_t& center_value)
{
	if (x >= 0 && x < low_data.cols && y >= 0 && y < low_data.rows)
	{
		uint8_t v = (float)low_data.at<uint8_t>(y, x);
		return (v > center_value);
	};
	
	return 0;
}
//-------------------------------------------------------------------------

int32_t SNDigitsComparator::CompareLBP(const cv::Mat& m1, const cv::Mat& m2)
{
	cv::Mat lbp1;
	cv::Mat lbp2;

	CalcLBP(m1, lbp1);
	CalcLBP(m2, lbp2);

	int32_t sum = 0;
	for (int i = 0; i < lbp1.cols; ++i)
	{
		if (lbp1.at<uint8_t>(i) == lbp2.at<uint8_t>(i))
			sum++;
	}

	return sum * 100 / lbp1.cols;
}
//-------------------------------------------------------------------------

void SNDigitsComparator::CompareLBP(const cv::Mat& image, SNDigitsComparatorResults& results)
{
	cv::Mat resized_image;

	cvtColor(image, resized_image, CV_RGB2GRAY);
	cv::resize(resized_image, resized_image, WindowSize);
	//cv::threshold(resized_image, resized_image, 128, 255, CV_THRESH_OTSU);
	
	for (auto& r : Templates)
	{
		int32_t res = CompareLBP(resized_image, r.second);
		results[r.first] = res;
	}
}
//----------------------------------------------------------------------