#ifndef SNANNPredictor_h__
#define SNANNPredictor_h__
//-------------------------------------------------------------------------------
#include <string>
#include "SNANNFeatureEvaluator.h"
#include "SNNumberRecognizerDefines.h"
//-------------------------------------------------------------------------------
namespace SNNumberRecognizer
{
	class SNANNPredictor
	{
	public:
		SNANNPredictor();
		~SNANNPredictor();
		void Load(SNANNConfigMap& ann_config);
		int32_t Predict(AlphabetTypes type, const ANNClassItem& features, float& weight);
		int32_t Predict(AlphabetTypes type, const cv::Mat& image, SNANNFeatureEvaluator& eval, float& weight);
		//void Predict(const cv::Mat& image, SNANNFeatureEvaluator& eval, cv::Size win_size, SNANNPredictionResults& results);
		//void Predict(const cv::Mat& image, SNANNFeatureEvaluator& eval, SNPlateTemplate& templ, SNANNPredictionResults& results);
		void CalcDetectionVariants(SNANNPredictionResults& results);
	private:
		ANNAlphabets Alphabets;
	};
	//-------------------------------------------------------------------------------
}
#endif // SNANNPredictor_h__



