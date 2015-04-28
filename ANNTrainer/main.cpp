
#include <QtCore/QCoreApplication>
#include "../Common/SNTestNumbersLoader.h"
#include "../Common/SNANNPredictor.h"
#include "SNANNTrainer.h"
#include <QDebug>
#include "../Common/SNANNConfigLoader.h"
#include "SNLBPTrainer.h"
#include <QTime>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QFile>

void Evaluate(const SNNumberRecognizer::ANNAlphabets& alphabets, SNNumberRecognizer::SNANNConfigMap config)
{
	SNNumberRecognizer::SNANNPredictor pred;
	int32_t total_count = 0;
	int32_t total_correct_count = 0;
	int32_t total_incorrect_count = 0;
	int32_t total_unrecognized_count = 0;

	pred.Load(config);

	uint32_t total_evaluations = 0;

	QTime time;
	time.start();

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

				total_evaluations++;
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

	qDebug() << QString("Overall performance: correct %1 unrec %2 incorrect %3 TotalTime %4 Total evaluations %5").arg(total_correct_count * 100.f / total_count).arg(total_unrecognized_count * 100.f / total_count).arg(total_incorrect_count * 100.f / total_count).arg(time.elapsed()).arg(total_evaluations);

	int r = 0;
}
//--------------------------------------------------------------------------------------

void TestPlate(std::string filename, SNNumberRecognizer::SNANNConfigMap config)
{
	cv::Mat color_image = cv::imread(filename);
	cv::Mat gray_image;
	cvtColor(color_image, gray_image, CV_RGB2GRAY);
	cv::resize(color_image, color_image, cv::Size(color_image.cols * 10, color_image.rows * 10));
	int window_w = 6;
	int window_h = 12;

	SNANNFeatureEvaluator eval;

	SNNumberRecognizer::SNANNPredictor pred;
	pred.Load(config);

	QTime time;
	time.start();

	for (int y = 0; y < gray_image.rows - window_h; ++y)
	{
		for (int x = 0; x < gray_image.cols - window_w; ++x)
		{
			cv::Mat window = cv::Mat(gray_image, cv::Rect(x, y, window_w, window_h));
			float weight;
			/*int cl = pred.Predict(SNNumberRecognizer::DigitsAlphabet, window, eval, weight);
			if (cl >= 0 && weight > 0.8)
			{
				cv::rectangle(color_image, cv::Rect(x * 10, y * 10, window_w * 10, window_h * 10), cv::Scalar(255, 0, 0));
			}*/
		}
	}

	int t = time.elapsed();

	qDebug() << t;

	int r = 0;
}
//--------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	SNTestNumbersLoader TNL;

	SNNumberRecognizer::SNLBPTrainer lbp_trainer;

	cv::FileStorage fs("data.xml", cv::FileStorage::MEMORY | cv::FileStorage::WRITE);

	SNNumberRecognizer::ANNAlphabets train_alphabets;

	SNANNFeatureEvaluator eval;

	QString train_alph_config_file = "e:\\symbols6\\alphabets_config.xml";
	TNL.LoadSymbols(train_alphabets, train_alph_config_file);
	TNL.EvaluateFeatures(train_alphabets, eval);

	//lbp_trainer.Train(train_alphabets);

	SNNumberRecognizer::ANNTrainer trainer;

	SNNumberRecognizer::SNANNConfigMap config;

	qDebug() << "Training begins";
	trainer.Train(train_alphabets, 100, config);
	qDebug() << "Training finished";

	SNNumberRecognizer::SNANNConfigLoader config_loader;

	std::string res = config_loader.Save(config);

	QFile f1("test.ann");
	f1.open(QIODevice::WriteOnly);

	f1.write(res.c_str(), res.size());

	f1.close();

	SNNumberRecognizer::ANNAlphabets eval_alphabets;

	QString eval_alph_config_file = "e:\\symbols4\\alphabets_config.xml";
	TNL.LoadSymbols(eval_alphabets, eval_alph_config_file);
	TNL.EvaluateFeatures(eval_alphabets, eval);

	Evaluate(eval_alphabets, config);

	return a.exec();
}

