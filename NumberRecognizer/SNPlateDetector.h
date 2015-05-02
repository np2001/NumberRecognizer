#ifndef SNPlateDetector_h__
#define SNPlateDetector_h__
//------------------------------------------------------------------------------------
#include <opencv2\core\core.hpp>
#include <opencv2\objdetect\objdetect.hpp>
#include <windows.h>
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
	//void Detect(const cv::Mat& image, cv::Size min_size, cv::Size max_size);
public:
	float MinRelativePlateWidth;
	float MaxRelativePlateWidth;
	float AspectRatio;

private:
	void Init(const cv::Mat& image);
private:
	cv::CascadeClassifier Classifier;
	int ImageWidth;
	int ImageHeight;
	cv::Size MinPlateSize;
	cv::Size MaxPlateSize;
};
//------------------------------------------------------------------------------------

#endif // SNPlateDetector_h__
