#ifndef SNANNFeatureEvaluator_h__
#define SNANNFeatureEvaluator_h__
//----------------------------------------------------------------------
#include <opencv2/core/core.hpp>
#include <stdint.h>
//----------------------------------------------------------------------

class SNANNFeatureEvaluator
{
public:
	SNANNFeatureEvaluator();
	~SNANNFeatureEvaluator();
	virtual cv::Mat Features(const cv::Mat& gray_image);
private:
	void LBPBit(const cv::Mat& low_data, int32_t x, int32_t y, const uint8_t& center_value, const uint8_t& bitmask, uint8_t& value);
};
//----------------------------------------------------------------------
#endif // SNANNFeatureEvaluator_h__

