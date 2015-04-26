#ifndef SNLocalContraster_h__
#define SNLocalContraster_h__
//---------------------------------------------------------
#include <opencv2\core\core.hpp>
#include <stdint.h>
//---------------------------------------------------------

class SNLocalContraster
{
public:
	SNLocalContraster();
	~SNLocalContraster();
	cv::Mat ProcessGray(const cv::Mat& gray_image);
	cv::Mat ProcessRGB24(const cv::Mat& rgb24_image);

private:
	bool IsPointDarker(const cv::Mat& image, int32_t x, int32_t y, uint8_t central_value);
};
//---------------------------------------------------------
#endif // SNLocalContraster_h__
