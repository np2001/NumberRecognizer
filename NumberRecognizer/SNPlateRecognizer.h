#ifndef SNPlateRecognizer_h__
#define SNPlateRecognizer_h__
//---------------------------------------------------------------------
#include "..\Common\SNNumberRecognizerDefines.h"
#include "..\Common\SNANNConfigLoader.h"
#include "..\Common\SNANNPredictor.h"
#include "SNSegmentor.h"
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
		void RecognizePlate(SNPlate& plate, SNNumberVariants& variants, SNFramesToRelease& frames_to_release);
		bool InitRecognizer(const char* config);
		void CheckResults(const uint64_t& frame_id, SNFramesToRelease& frames_to_release, SNFinalResults& results);
		
	private:
		std::string RecognizeRegion(const cv::Mat& gray_image, const SNPlateModel& best_plate_model);
		void CalcTotalWeight(SNNumberStats& stats);
	private:
		SNANNConfigLoader ConfigLoader;
		SNANNPredictor Predictor;
		SNSegmentor Segmentor;
		SNModelMatcher ModelMatcher;
		SNANNFeatureEvaluator Eval;
		SNFormatMatcher FormatMatcher;
		SNStatsCombiner StatsCombiner;
	};
}
//---------------------------------------------------------------------



#endif // SNPlateRecognizer_h__
