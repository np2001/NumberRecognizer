#include "SNPlateDetector.h"
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "opencv2\video\background_segm.hpp"
//------------------------------------------------------------------------------------
namespace SNNumberRecognizer
{
	SNPlateDetector::SNPlateDetector()
	{
		MinRelativePlateWidth = 0.05f;
		MaxRelativePlateWidth = 0.1f;
		AspectRatio = 4.64f;
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

	void SNPlateDetector::Detect(const cv::Mat& image, const uint64_t& frame_id, SNPlates& objects)
	{
		cv::Mat mini_image;
		mini_image = image;

		/*cv::Mat ccc;
		cv::resize(image, ccc, cv::Size(image.cols / 8, image.rows / 8));

		cv::Mat canny_image;
		cv:Canny(ccc, canny_image, 128, 255);
		mog(canny_image, canny_image, 0.01);

		int erosion_size = 0;
		cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
		cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		cv::Point(erosion_size, erosion_size));
		cv::dilate(canny_image, canny_image, element);*/

		Init(mini_image);

		std::vector<cv::Rect> plate_rects;
		Classifier.detectMultiScale(mini_image, plate_rects, 1.1, 3, 0, MinPlateSize, MaxPlateSize);

		for (auto& pr : plate_rects)
		{
			SNPlate plate_rect;
			plate_rect.FrameID = frame_id;

			/*pr.x = pr.x * 2;
			pr.y = pr.y * 2;
			pr.width = pr.width * 2;
			pr.height = pr.height * 2;*/

			plate_rect.GlobalRect = pr;

			cv::Point2f plate_center = cv::Point2f(plate_rect.GlobalRect.tl().x + plate_rect.GlobalRect.width / 2.0f, plate_rect.GlobalRect.tl().y + plate_rect.GlobalRect.height / 2.0f);
			
			plate_rect.GlobalRect.width = (int32_t)(plate_rect.GlobalRect.width * 1.5f);
			plate_rect.GlobalRect.height = (int32_t)(plate_rect.GlobalRect.height * 1.5f);

			plate_rect.GlobalRect.x = (int32_t)((plate_center.x - plate_rect.GlobalRect.width / 2.0f) + 0.5);
			plate_rect.GlobalRect.y = (int32_t)((plate_center.y - plate_rect.GlobalRect.height / 2.0f) + 0.5);

			plate_rect.GlobalRect &= cv::Rect(0, 0, image.cols - 1, image.rows - 1);
			plate_rect.PlateImage = cv::Mat(mini_image, plate_rect.GlobalRect).clone();
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
			MinPlateSize = cv::Size((int32_t)(MinRelativePlateWidth * ImageWidth), (int32_t)(MinRelativePlateWidth / AspectRatio * ImageWidth));
			MaxPlateSize = cv::Size((int32_t)(MaxRelativePlateWidth * ImageWidth), (int32_t)(MaxRelativePlateWidth / AspectRatio * ImageWidth));
		}
	}
}
//------------------------------------------------------------------------------------