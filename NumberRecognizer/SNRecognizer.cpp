#include "SNRecognizer.h"
#include "..\Common\SNStringEncryptor.h"
#include "..\Common\plate_detector_cascade.h"
#include "opencv2\imgproc\imgproc.hpp"
#include "SNImageDebugger.h"
#include "SNRecognizerProcessor.h"

void* __stdcall SNNumrecCreate()
{
	return new SNNumberRecognizer::SNRecognizer();
};
//---------------------------------------------------------------------------

void __stdcall SNNumrecDelete(void* instance)
{
	SNNumberRecognizer::SNRecognizer* inst = (SNNumberRecognizer::SNRecognizer*)instance;
	delete inst;
};
//---------------------------------------------------------------------------

bool(__stdcall SNNumrecProcess)  (void* instance, char* buffer, int out_width, int out_height)
{
	return false;
};
//---------------------------------------------------------------------------

namespace SNNumberRecognizer
{
	SNRecognizer::SNRecognizer()
	{
		SNStringEncryptor se;
		se.Crypt(plate_detector_cascade);
		PlateDetector.LoadCascade((char*)plate_detector_cascade);

		LastFrameID = 0;

		RecProcessor = new SNRecognizerProcessor(this);
	}
	//--------------------------------------------------------------

	SNRecognizer::~SNRecognizer()
	{
		delete RecProcessor;
	}
	//--------------------------------------------------------------

	bool SNRecognizer::Process(const SNNumberRecognizerInputFrame& frame)
	{
		LastFrameID = frame.FrameID;

		cv::Rect roi_rect = cv::Rect(frame.ROIX * frame.Width, frame.ROIY * frame.Height, frame.ROIWidth * frame.Width, frame.ROIHeight * frame.Height);

		cv::Mat image = cv::Mat(frame.Height, frame.Width, CV_8UC4, frame.RGB32Image);

		cv::Mat roi_image = cv::Mat(image, roi_rect);
		
		cv::Mat roi_gray_image;

		cv::cvtColor(roi_image, roi_gray_image, CV_RGBA2GRAY);

		cv::Rect roi_gray_image_rect = cv::Rect(0, 0, roi_gray_image.cols, roi_gray_image.rows);

		SNPlates plates;
		PlateDetector.Detect(roi_gray_image, frame.FrameID, plates);

		bool retain_image = plates.size() > 0;

		Mutex.lock();
		for (auto& plate : plates)
		{
			NotRecognizedPlates.push_back(plate);
		}

		Mutex.unlock();

		if (retain_image)
		{
			ImageRetainMap[frame.FrameID] += plates.size();
			char c[100];
			sprintf_s(c, 100, "Image retained. Left: %i\r\n", ImageRetainMap.size());
			OutputDebugStringA(c);
		}
		
		ReleaseFrames();

		for (auto fr : FinalResults)
		{
			if (fr.Weight > 10)
			{
				OutputDebugStringA("Final result\r\n");

				char c[100];
				sprintf_s(c, 100, "%s %2.2f\r\n", fr.Number.c_str(), fr.Weight);
				OutputDebugStringA(c);
			}
		}

		FinalResults.clear();

		return retain_image;
	}
	//--------------------------------------------------------------

	void SNRecognizer::ReleaseFrames()
{
		for (auto& f : FramesToRelease)
		{
			auto i = ImageRetainMap.find(f);
			if (i != ImageRetainMap.end())
			{
				i->second--;
				if (i->second == 0)
				{
					char c[100];
					sprintf_s(c, 100, "Image released. Left: %i\r\n", ImageRetainMap.size() - 1);
					OutputDebugStringA(c);
					ImageRetainMap.erase(i);
				}
			}
			else
			{
				OutputDebugStringA("No frame to release\r\n");
			}
		}

		FramesToRelease.clear();
	}
	//--------------------------------------------------------------

	bool SNRecognizer::GetPlateToRecognize(SNRecognizerProcessorQuery& query)
	{
		bool res = false;
		Mutex.lock();
		
		query.LastFrameID = LastFrameID;

		for (auto& ftr : query.FramesToRelease)
		{
			FramesToRelease.push_back(ftr);
		}

		for (auto& fr : query.FinalResults)
		{
			FinalResults.push_back(fr);
		}

		if (!NotRecognizedPlates.empty())
		{
			query.PlateToRecognize = NotRecognizedPlates.front();
			NotRecognizedPlates.pop_front();

			res = true;
		}

		Mutex.unlock();

		return res;
	}
	//--------------------------------------------------------------
};
