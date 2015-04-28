#ifndef SNANNPredictor_h__
#define SNANNPredictor_h__
//-------------------------------------------------------------------------------
#include <string>
#include "SNANNFeatureEvaluator.h"
#include "SNNumberRecognizerDefines.h"
//-------------------------------------------------------------------------------

namespace SNNumberRecognizer
{
	struct ANNPredictionResult
	{
		char Symbol;
		float Weight;
	};

	struct ANNPredictionResults : public std::list<ANNPredictionResult>
	{
		void Sort()
		{
			sort([](const ANNPredictionResult & a, const ANNPredictionResult & b) -> bool
			{
				return a.Weight > b.Weight;
			});
		}
	};

	class SNANNPredictor
	{
	public:
		SNANNPredictor();
		~SNANNPredictor();
		void Load(SNANNConfigMap& ann_config);
		int32_t Predict(AlphabetTypes type, const ANNClassItem& features, float& weight);
		void Predict(AlphabetTypes type, const cv::Mat& image, SNANNFeatureEvaluator& eval, ANNPredictionResults& results);
	private:
		ANNAlphabets Alphabets;
	};
	//-------------------------------------------------------------------------------
}
#endif // SNANNPredictor_h__



