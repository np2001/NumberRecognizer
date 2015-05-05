#include "SNPlateDetector.h"
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
//------------------------------------------------------------------------------------
namespace SNNumberRecognizer
{
	SNPlateDetector::SNPlateDetector()
	{
		MinRelativePlateWidth = 0.05;
		MaxRelativePlateWidth = 0.1;
		AspectRatio = 4.64;
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

	void SNPlateDetector::Detect(const cv::Mat& image, const uint64_t& frame_id, SNPlateRects& objects)
	{
		Init(image);

		std::vector<cv::Rect> plate_rects;
		Classifier.detectMultiScale(image, plate_rects, 1.1, 3, 0, MinPlateSize, MaxPlateSize);

		for (auto& pr : plate_rects)
		{
			SNPlateRect plate_rect;
			plate_rect.FrameID = frame_id;
			plate_rect.GlobalRect = pr;

			cv::Point2f plate_center = cv::Point2f(plate_rect.GlobalRect.tl().x + plate_rect.GlobalRect.width / 2.0f, plate_rect.GlobalRect.tl().y + plate_rect.GlobalRect.height / 2.0f);
			
			plate_rect.GlobalRect.width *= 1.5f;
			plate_rect.GlobalRect.height *= 1.5f;

			plate_rect.GlobalRect.x = (plate_center.x - plate_rect.GlobalRect.width / 2.0f) + 0.5;
			plate_rect.GlobalRect.y = (plate_center.y - plate_rect.GlobalRect.height / 2.0f) + 0.5;

			plate_rect.GlobalRect &= cv::Rect(0, 0, image.cols - 1, image.rows - 1);
			plate_rect.PlateImage = cv::Mat(image, plate_rect.GlobalRect).clone();
			plate_rect.LocalRect = cv::Rect(pr.x - plate_rect.GlobalRect.x, pr.y - plate_rect.GlobalRect.y, pr.width, pr.height);
			plate_rect.LocalRect &= cv::Rect(0, 0, plate_rect.PlateImage.cols - 1, plate_rect.PlateImage.rows - 1);

			objects.push_back(plate_rect);
		}
	}
	//------------------------------------------------------------------------------------

	void SNPlateDetector::Init(const cv::Mat& image)
	{
		if (image.cols != ImageWidth || image.rows != ImageHeight)
		{
			ImageWidth = image.cols;
			ImageHeight = image.rows;
			MinPlateSize = cv::Size(MinRelativePlateWidth * ImageWidth, MinRelativePlateWidth / AspectRatio * ImageWidth);
			MaxPlateSize = cv::Size(MaxRelativePlateWidth * ImageWidth, MaxRelativePlateWidth / AspectRatio * ImageWidth);
		}
	}
}
//------------------------------------------------------------------------------------