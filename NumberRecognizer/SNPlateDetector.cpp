#include "SNPlateDetector.h"
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
//------------------------------------------------------------------------------------

SNPlateDetector::SNPlateDetector()
{
	MinRelativePlateWidth = 0.1;
	MaxRelativePlateWidth = 0.3;
	AspectRatio = 3.58;
	ImageWidth = 0;
	ImageHeight = 0;
}
//------------------------------------------------------------------------------------

SNPlateDetector::~SNPlateDetector()
{
}
//------------------------------------------------------------------------------------

void SNPlateDetector::LoadCascade(const std::string& cascade)
{
	cv::FileStorage fs(cascade, cv::FileStorage::MEMORY);

	cv::FileNode fn = fs["cascade"];

	Classifier.read(fn);
}
//------------------------------------------------------------------------------------

void SNPlateDetector::Detect(const cv::Mat& image, SNPlateRects& plates)
{
	Init(image);

	Classifier.detectMultiScale(image, plates.PlateRects, 1.1, 3, 0, MinPlateSize, MaxPlateSize);

	for (auto& pr : plates.PlateRects)
	{
		cv::Point2f plate_center = cv::Point2f(pr.tl().x + pr.width / 2.0f, pr.tl().y + pr.height / 2.0f);
		pr.width *= 1.5f;
		pr.height *= 1.5f;

		pr.x = (plate_center.x - pr.width / 2.0f) + 0.5;
		pr.y = (plate_center.y - pr.height / 2.0f) + 0.5;
	}
}
//------------------------------------------------------------------------------------

void SNPlateDetector::Init(const cv::Mat& image)
{
	if (image.cols != ImageWidth || image.rows != ImageHeight)
	{
		ImageWidth = image.cols;
		ImageHeight = image.rows;
		MinPlateSize = cv::Size(MinRelativePlateWidth * ImageWidth, MinRelativePlateWidth / AspectRatio * ImageHeight);
		MaxPlateSize = cv::Size(MaxRelativePlateWidth * ImageWidth, MaxRelativePlateWidth / AspectRatio * ImageHeight);
	}
}
//------------------------------------------------------------------------------------

