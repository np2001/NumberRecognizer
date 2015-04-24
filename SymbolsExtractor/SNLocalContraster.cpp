#include "SNLocalContraster.h"
//-----------------------------------------------------------------
SNLocalContraster::SNLocalContraster()
{

}
//-----------------------------------------------------------------

SNLocalContraster::~SNLocalContraster()
{

}
//-----------------------------------------------------------------

cv::Mat SNLocalContraster::Process(const cv::Mat& gray_image)
{
	cv::Mat res = gray_image.clone();

	for (int32_t y = 0; y < gray_image.rows; ++y)
	{
		for (int32_t x = 0; x < gray_image.cols; ++x)
		{
			int32_t darker_count = 0;
			uint8_t central_value = gray_image.at<uint8_t>(y, x);
			darker_count += IsPointDarker(gray_image, x - 1, y - 1, central_value) ? 1 : 0;
			darker_count += IsPointDarker(gray_image, x - 0, y - 1, central_value) ? 1 : 0;
			darker_count += IsPointDarker(gray_image, x + 1, y - 1, central_value) ? 1 : 0;
			darker_count += IsPointDarker(gray_image, x + 1, y - 0, central_value) ? 1 : 0;
			darker_count += IsPointDarker(gray_image, x + 1, y + 1, central_value) ? 1 : 0;
			darker_count += IsPointDarker(gray_image, x - 0, y + 1, central_value) ? 1 : 0;
			darker_count += IsPointDarker(gray_image, x - 1, y + 1, central_value) ? 1 : 0;
			darker_count += IsPointDarker(gray_image, x - 1, y - 0, central_value) ? 1 : 0;

			res.at<uint8_t>(y, x) = darker_count < 3 ? 255 : 0;
		}
	}

	return res;
}
//-----------------------------------------------------------------

bool SNLocalContraster::IsPointDarker(const cv::Mat& image, int32_t x, int32_t y, uint8_t central_value)
{
	if (x >= 0 && x < image.cols && y >= 0 && y < image.rows)
	{
		uint8_t offset_value = image.at<uint8_t>(y, x);
		return offset_value < central_value;
	}
	else
	{
		return false;
	}
}
//-----------------------------------------------------------------