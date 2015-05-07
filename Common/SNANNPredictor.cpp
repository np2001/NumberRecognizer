#include "SNANNPredictor.h"
#include "opencv2\core\core_c.h"
//#include <QDebug>
//#include "SNANNLearningData.h"
//-------------------------------------------------------------------------------
namespace SNNumberRecognizer
{
	SNANNPredictor::SNANNPredictor()
	{
	}
	//-------------------------------------------------------------------------------

	SNANNPredictor::~SNANNPredictor()
	{
	}
	//-------------------------------------------------------------------------------

	void SNANNPredictor::Load(SNANNConfigMap& ann_config)
	{
		for (auto alph : ann_config)
		{
			cv::FileStorage fs(alph.second.ConfigString, cv::FileStorage::MEMORY);

			CvFileStorage* fss = *fs;

			CvFileNode* model_node = 0;

			CvFileNode* root = cvGetRootFileNode(fss);
			if (root->data.seq->total > 0)
				model_node = (CvFileNode*)cvGetSeqElem(root->data.seq, 0);

			Alphabets[alph.first] = ANNAlphabet();
			Alphabets[alph.first].ANN.read(fss, model_node);
			Alphabets[alph.first].SymbolClasses = alph.second.CharClassIDs;
		}
	}
	//-------------------------------------------------------------------------------

	int32_t SNANNPredictor::Predict(AlphabetTypes alphabet_type, const ANNClassItem& features, float& weight)
	{
		cv::Mat output;
		Alphabets[alphabet_type].ANN.predict(features.Features, output);

		float max_response = -1000;
		int max_response_idx = -1;

		for (int i = 0; i < output.cols; ++i)
		{
			float res = output.at<float>(i);
			if (res> 0 && res > max_response)
			{
				max_response = res;
				max_response_idx = i;
			}
		}

		weight = max_response;

		return max_response_idx;
	};
	//-------------------------------------------------------------------------------

	void SNANNPredictor::Predict(AlphabetTypes type, const cv::Mat& image, SNANNFeatureEvaluator& eval, ANNPredictionResults& results)
	{
		ANNClassItem features;
		features.Features = eval.Features(image);

		//float weight;
		//int32_t class_id = Predict(type, features, weight);

		cv::Mat output;
		Alphabets[type].ANN.predict(features.Features, output);

		for (int i = 0; i < output.cols; ++i)
		{
			float weight = output.at<float>(i);

			ANNPredictionResult res;
			res.Weight = weight;
			ANNSymbolClassesMap::iterator iter = Alphabets[type].SymbolClasses.find(i);
			if (iter == Alphabets[type].SymbolClasses.end())
			{
				res.Symbol = '*';
			}
			else
			{
				res.Symbol = iter->second.DisplaySymbol;
			}

			results.push_back(res);
		}

		results.Sort();
	}
	//-------------------------------------------------------------------------------
}