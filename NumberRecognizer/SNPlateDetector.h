#ifndef SNPlateDetector_h__
#define SNPlateDetector_h__
//------------------------------------------------------------------------------------
#include <opencv2\core\core.hpp>
#include <opencv2\objdetect\objdetect.hpp>
#include <windows.h>
#include <stdint.h>
#include "..\Common\SNNumberRecognizerDefines.h"
#include "opencv2\video\background_segm.hpp"
//------------------------------------------------------------------------------------
namespace SNNumberRecognizer
{
	class SNPlateDetector
	{
	public:
		SNPlateDetector();
		~SNPlateDetector();
		void LoadCascade(const std::string& cascade);

		void Detect(const cv::Mat& image, const uint64_t& frame_id, SNPlates& objects);
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
		cv::BackgroundSubtractorMOG mog;

	};
}
//------------------------------------------------------------------------------------

#endif // SNPlateDetector_h__
