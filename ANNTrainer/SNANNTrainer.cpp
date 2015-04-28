#include "SNANNTrainer.h"
//-------------------------------------------------------------------------
namespace SNNumberRecognizer
{
	ANNTrainer::ANNTrainer()
	{
	}
	//-------------------------------------------------------------------------

	ANNTrainer::~ANNTrainer()
	{
	}
	//-------------------------------------------------------------------------

	void ANNTrainer::Train(ANNAlphabets& data, int num_layers, SNANNConfigMap& ann_config)
	{
		for (auto alphabet : data)
		{
			int total_data_count = 0;
			for (auto d : alphabet.second.SymbolClasses)
			{
				total_data_count += d.second.size();
			}

			int features_size = alphabet.second.SymbolClasses.begin()->second.front().Features.cols;

			cv::Mat train_data = cv::Mat(total_data_count, features_size, CV_32FC1);
		
			int num_classes = alphabet.second.SymbolClasses.size() - 1;

			cv::Mat train_classes;
			train_classes.create(total_data_count, num_classes, CV_32FC1);

			int idx = 0;
			for (auto symbol_class : alphabet.second.SymbolClasses)
			{
				ann_config[alphabet.first].CharClassIDs[symbol_class.first].DisplaySymbol = symbol_class.second.DisplaySymbol;

				for (auto i : symbol_class.second)
				{
					for (int j = 0; j < features_size; ++j)
					{
						train_data.at<float>(idx, j) = i.Features.at<float>(j);
					}

					for (int k = 0; k < num_classes; k++)
					{
						//If class of data i is same than a k class
						if (k == symbol_class.first /*&& symbol_class.second.DisplaySymbol != '*' && symbol_class.second.DisplaySymbol != '-'*/)
							train_classes.at<float>(idx, k) = 1;
						else
							train_classes.at<float>(idx, k) = -1;
					}

					idx++;
				}
			}

			cv::Mat layers(1, 3, CV_32SC1);
			layers.at<int>(0) = train_data.cols;
			layers.at<int>(1) = num_layers;
			layers.at<int>(2) = num_classes;
			ANN.create(layers, CvANN_MLP::SIGMOID_SYM, 1, 1);

			cv::Mat weights(1, train_data.rows, CV_32FC1, cv::Scalar::all(1));

			ANN.train(train_data, train_classes, weights);

			std::string res;

			cv::FileStorage fs("data.xml", cv::FileStorage::MEMORY | cv::FileStorage::WRITE);

			CvFileStorage* fss = *fs;
			
			ANN.write(fss, "SNANN");

			res = fs.releaseAndGetString();

			ann_config[alphabet.first].ConfigString = res;
		}
	}
}
//-------------------------------------------------------------------------