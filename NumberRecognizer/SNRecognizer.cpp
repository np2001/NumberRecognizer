#include "SNRecognizer.h"
#include "..\Common\ann_config.h"
#include "..\Common\SNStringEncryptor.h"
#include "..\Common\plate_detector_cascade.h"
#include "opencv2\imgproc\imgproc.hpp"

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

	void SNRecognizer::Process(const SNNumberRecognizerInputFrame& frame)
	{
		cv::Mat image = cv::Mat(frame.Height, frame.Width, CV_8UC4, frame.RGB32Image);

		cv::Mat gray_image;

		cv::cvtColor(image, gray_image, CV_RGBA2GRAY);

		SNPlateRects rects;
		PlateDetector.Detect(gray_image, rects);

		cv::Rect image_rect = cv::Rect(0, 0, frame.Width - 1, frame.Height - 1);

		for (auto plate_rect : rects.PlateRects)
		{
			plate_rect &= image_rect;
		
			cv::Mat plate_image = cv::Mat(gray_image, plate_rect);

			SNFigureGroups fgs;
			SNNumberVariants variants;
			PlateRecognizer.RecognizePlate(frame.FrameID, plate_rect, plate_image, fgs, variants);

			cv::Mat debug_image;
			PlateRecognizer.DebugFigureGroups(plate_image, fgs, debug_image, 2);

			int r = 0;
		}

		PlateRecognizer.CheckResults(frame.FrameID);
	}
	//--------------------------------------------------------------
};
