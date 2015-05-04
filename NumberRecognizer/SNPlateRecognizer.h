#ifndef SNPlateRecognizer_h__
#define SNPlateRecognizer_h__
//---------------------------------------------------------------------
#include "..\Common\SNNumberRecognizerDefines.h"
#include "..\Common\SNANNConfigLoader.h"
#include "..\Common\SNANNPredictor.h"
#include "SNMasterSegmentor.h"
#include "SNModelMatcher.h"
#include "SNFormatMatcher.h"
#include "SNStatsCombiner.h"
//---------------------------------------------------------------------

namespace SNNumberRecognizer
{
	class SNPlateRecognizer
	{
	public:
		SNPlateRecognizer();
		~SNPlateRecognizer();
		void RecognizePlate(uint64_t frame_id, const cv::Rect& plate_rect, const cv::Mat& plate_image, SNFigureGroups& fgs, SNNumberVariants& variants);
		bool InitRecognizer(const char* config);
		void CheckResults(const uint64_t& frame_id);
	private:
		std::string RecognizeRegion(const cv::Mat& gray_image, const SNPlateModel& best_plate_model);
	private:
		SNANNConfigLoader ConfigLoader;
		SNANNPredictor Predictor;
		SNMasterSegmentor Segmentor;
		SNModelMatcher ModelMatcher;
		SNANNFeatureEvaluator Eval;
		SNFormatMatcher FormatMatcher;
		SNStatsCombiner StatsCombiner;
	};
}
//---------------------------------------------------------------------

#endif // SNPlateRecognizer_h__
