#ifndef SNPlateRecognizer_h__
#define SNPlateRecognizer_h__
//---------------------------------------------------------------------
#include "..\Common\SNNumberRecognizerDefines.h"
#include "..\Common\SNANNConfigLoader.h"
#include "..\Common\SNANNPredictor.h"
#include "SNMasterSegmentor.h"
#include "SNModelMatcher.h"
#include "SNFormatMatcher.h"
//---------------------------------------------------------------------

namespace SNNumberRecognizer
{
	class SNPlateRecognizer
	{
	public:
		SNPlateRecognizer();
		~SNPlateRecognizer();
		void RecognizePlate(const cv::Mat& image, SNNumberVariants& variants);
		bool InitRecognizer(const char* config);
	private:
		std::string RecognizeRegion(const cv::Mat& gray_image, const SNPlateModel& best_plate_model);
	private:
		SNANNConfigLoader ConfigLoader;
		SNANNPredictor Predictor;
		SNMasterSegmentor Segmentor;
		SNModelMatcher ModelMatcher;
		SNANNFeatureEvaluator Eval;
		SNFormatMatcher FormatMatcher;
	};
}
//---------------------------------------------------------------------

#endif // SNPlateRecognizer_h__
