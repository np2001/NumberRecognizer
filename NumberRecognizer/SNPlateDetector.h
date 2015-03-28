#ifndef SNPlateDetector_h__
#define SNPlateDetector_h__
//------------------------------------------------------------------------------------
#include <opencv2\objdetect\objdetect.hpp>
#include <list>
#include <stdint.h>
//------------------------------------------------------------------------------------

struct SNPlateRects
{
	std::vector<cv::Rect> PlateRects;
};
//------------------------------------------------------------------------------------

class SNPlateDetector
{
public:
	SNPlateDetector();
	~SNPlateDetector();
	void LoadCascade(const std::string& cascade);
	
	void Detect(const cv::Mat& image, SNPlateRects& objects);
private:
	void Init(const cv::Mat& image);
private:
	cv::CascadeClassifier Classifier;
	float MinRelativePlateWidth;
	float MaxRelativePlateWidth;
	float AspectRatio;
	int ImageWidth;
	int ImageHeight;
	cv::Size MinPlateSize;
	cv::Size MaxPlateSize;
};
//------------------------------------------------------------------------------------

#endif // SNPlateDetector_h__
