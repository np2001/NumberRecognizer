
#include <QtCore/QCoreApplication>
#include "../Common/SNTestNumbersLoader.h"
#include "../Common/SNANNPredictor.h"
#include "SNANNTrainer.h"
#include <QDebug>
#include "../Common/SNANNConfigLoader.h"

void Evaluate(const SNNumberRecognizer::ANNAlphabets& alphabets, SNNumberRecognizer::SNANNConfigMap config)
{
	SNNumberRecognizer::SNANNPredictor pred;
	int32_t total_count = 0;
	int32_t total_correct_count = 0;
	int32_t total_incorrect_count = 0;
	int32_t total_unrecognized_count = 0;

	pred.Load(config);

	for (auto alph : alphabets)
	{
		int32_t alphabet_total_count = 0;
		int32_t alphabet_correct_count = 0;
		int32_t alphabet_incorrect_count = 0;
		int32_t alphabet_unrecognized_count = 0;


		for (auto sym : alph.second.SymbolClasses)
		{
			int32_t class_total_count = 0;
			int32_t class_correct_count = 0;
			int32_t class_incorrect_count = 0;
			int32_t class_unrecognized_count = 0;

			int class_id = sym.first;

			std::map<int, int> class_stats_count;

			for (auto i : sym.second)
			{
				float weight;
				int32_t class_id2 = pred.Predict(alph.first, i, weight);
				class_stats_count[class_id2]++;

				//if (class_id >= 0)
				{
					total_count++;
					class_total_count++;
					alphabet_total_count++;
				}

				if (class_id2 == class_id)
				{
					total_correct_count++;
					class_correct_count++;
					alphabet_correct_count++;
				}
				else
				{
					if (class_id2 >= 0 || class_id == -1)
					{
						total_incorrect_count++;
						class_incorrect_count++;
						alphabet_incorrect_count++;
					}
					else
					{
						total_unrecognized_count++;
						class_unrecognized_count++;
						alphabet_unrecognized_count++;
					}
				}
			}

			qDebug() << QString("%1 C: %2 UR %3 IC: %4").arg(sym.second.DisplaySymbol).arg(class_correct_count * 100.f / class_total_count).arg(class_unrecognized_count * 100.f / class_total_count).arg(class_incorrect_count * 100.f / class_total_count);
		}

		qDebug() << QString("Alphabet %1 performance: correct %2 unrec %3 incorrect %4").arg(alph.first).arg(alphabet_correct_count * 100.f / alphabet_total_count).arg(alphabet_unrecognized_count * 100.f / alphabet_total_count).arg(alphabet_incorrect_count * 100.f / alphabet_total_count);
	}

	qDebug() << QString("Overall performance: correct %1 unrec %2 incorrect %3").arg(total_correct_count * 100.f / total_count).arg(total_unrecognized_count * 100.f / total_count).arg(total_incorrect_count * 100.f / total_count);

	int r = 0;
}
//--------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	SNTestNumbersLoader TNL;

	cv::FileStorage fs("data.xml", cv::FileStorage::MEMORY | cv::FileStorage::WRITE);

	SNNumberRecognizer::ANNAlphabets train_alphabets;

	SNANNFeatureEvaluator eval;

	QString train_alph_config_file = "e:\\symbols4\\alphabets_config.xml";
	TNL.LoadSymbols(train_alphabets, train_alph_config_file);
	TNL.EvaluateFeatures(train_alphabets, eval);

	SNNumberRecognizer::ANNTrainer trainer;

	SNNumberRecognizer::SNANNConfigMap config;

	SNNumberRecognizer::SNANNConfigLoader config_loader;
	
	qDebug() << "Training begins";
	trainer.Train(train_alphabets, 20, config);
	qDebug() << "Training finished";

	config_loader.Save(config);


	SNNumberRecognizer::ANNAlphabets eval_alphabets;

	QString eval_alph_config_file = "e:\\symbols\\alphabets_config.xml";
	TNL.LoadSymbols(eval_alphabets, eval_alph_config_file);
	TNL.EvaluateFeatures(eval_alphabets, eval);

	Evaluate(eval_alphabets, config);

	return a.exec();
}

