#include "SNRecognizer.h"
#include "..\Common\ann_config.h"
#include "..\Common\SNStringEncryptor.h"
#include "..\Common\plate_detector_cascade.h"
#include "opencv2\imgproc\imgproc.hpp"
#include "SNImageDebugger.h"

namespace SNNumberRecognizer
{
	SNRecognizer::SNRecognizer()
	{
		SNStringEncryptor se;

		se.Crypt(ann_config);

		PlateRecognizer.InitRecognizer(ann_config);

		se.Crypt(plate_detector_cascade);
		PlateDetector.LoadCascade((char*)plate_detector_cascade);

	}
	//--------------------------------------------------------------

	SNRecognizer::~SNRecognizer()
	{
	}
	//--------------------------------------------------------------

	bool SNRecognizer::Process(const SNNumberRecognizerInputFrame& frame)
	{
		cv::Rect roi_rect = cv::Rect(frame.ROIX * frame.Width, frame.ROIY * frame.Height, frame.ROIWidth * frame.Width, frame.ROIHeight * frame.Height);

		cv::Mat image = cv::Mat(frame.Height, frame.Width, CV_8UC4, frame.RGB32Image);

		cv::Mat roi_image = cv::Mat(image, roi_rect);
		
		cv::Mat roi_gray_image;

		cv::cvtColor(roi_image, roi_gray_image, CV_RGBA2GRAY);

		cv::Rect roi_gray_image_rect = cv::Rect(0, 0, roi_gray_image.cols, roi_gray_image.rows);

		SNPlates rects;
		PlateDetector.Detect(roi_gray_image, frame.FrameID, rects);

		bool retain_image = false;

		for (auto plate_rect : rects)
		{
			SNFigureGroups fgs;
			SNNumberVariants variants;
			PlateRecognizer.RecognizePlate(plate_rect, variants);

			retain_image = true;
		}

		if (retain_image)
			ImageRetainMap[frame.FrameID] += rects.size();

		PlateRecognizer.CheckResults(frame.FrameID);

		return retain_image;
	}
	//--------------------------------------------------------------
};
