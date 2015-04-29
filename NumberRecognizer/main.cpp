
#include <QtCore/QCoreApplication>
#include <opencv2/highgui/highgui.hpp>
#include "SNPlateDetector.h"
#include <QFile>
#include <QTextStream>
#include <opencv2/imgproc/imgproc.hpp>
#include <QTime>
#include <iostream>
#include <conio.h>
#include "../Common/SNANNConfigLoader.h"
#include "../Common/SNANNPredictor.h"
#include "../Common/SNNumberRecognizerDefines.h"
#include <QDebug>
#include "SNMasterRecognizer.h"
#include "SNMasterSegmentor.h"
#include "SNModelMatcher.h"
#include "SNFormatMatcher.h"

SNMasterRecognizer mr;
SNMasterSegmentor ms;

SNModelMatcher mm;

void LoadCascade(SNPlateDetector& pd, QString cascade_filename)
{
	QFile f(cascade_filename);
	f.open(QIODevice::ReadOnly);

	QTextStream ts(&f);
	QString s = ts.readAll();
	pd.LoadCascade(s.toStdString());

	f.close();
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	SNPlateDetector pd;

	SNANNFeatureEvaluator eval;
	SNFormatMatcher fm;

	SNNumberRecognizer::SNANNConfigLoader config_loader;

	SNNumberRecognizer::SNANNConfigMap test_config;
	QFile f("../ANNTrainer/best.ann");
	f.open(QIODevice::ReadOnly);

	QByteArray cfg = f.readAll();
	config_loader.Load(cfg.data(), test_config);
	f.close();

	SNNumberRecognizer::SNANNPredictor pred;
	pred.Load(test_config);

	cv::Mat image = cv::imread("f:/symbols6/plates/56.bmp", cv::IMREAD_GRAYSCALE);
	cv::resize(image, image, image.size() * 1);

	/*SNPlateModel pm;
	mm.BuildModel(cv::Rect(53, 13, 8, 11), cv::Rect(64, 9, 11, 14), pm);
	mm.DebugModel(image, pm);*/
	
	SNFigureGroups fgs;

	QTime time;
	time.start();
	ms.Segment(image, fgs, 100, 255, -1);
	
	cv::Mat debug_image;
	ms.DebugFigureGroups(image, fgs, debug_image, 2);

	mm.MatchModel(image, fgs);

	SNNumberStats stats;
	//msr.Process(image, fgs, stats);

	SNNumberRecognizer::ANNPredictionResults results;

	int t = time.elapsed();

	qDebug() << t;

	for (auto& fg : fgs)
	{
		stats.push_back(SNSymbolStats());

		for (auto& f : fg)
		{
			cv::Mat symbol = cv::Mat(image, cv::Rect(f.left(), f.top(), f.Width() + 1, f.Height() + 1)).clone();

			pred.Predict(SNNumberRecognizer::DigitsAlphabet, symbol, eval, results);

			stats.back().DigitsStats[results.front().Symbol] += results.front().Weight;
			results.clear();

			pred.Predict(SNNumberRecognizer::LettersAlphabet, symbol, eval, results);
			stats.back().LetterStats[results.front().Symbol] += results.front().Weight;
			results.clear();
		}
	}

	SNNumberVariants variants;
	fm.MatchNumbers(stats, variants);

	getch();

	LoadCascade(pd, "cascade_2.xml");
		int32_t total_count = 0;
	int32_t total_correct_count = 0;
	int32_t total_incorrect_count = 0;
	int32_t total_unrecognized_count = 0;


	cv::VideoCapture capture("e:/MailCloud/video_data_base/autonumbers/russia/num.avi");

	// current video frame
	cv::Mat frame;
	
	cv::Size window_size = cv::Size(8, 14);

	if (capture.isOpened())
	{
		bool first_frame = true;

		while (true)
		{
			if (!capture.read(frame))
				break;

			SNPlateRects rects;

			//25, 7, 33, 19

			QTime time;
			time.start();

			pd.Detect(frame, rects);
			
			cv::Rect image_rect = cv::Rect(0, 0, frame.cols - 1, frame.rows - 1);

			if (rects.PlateRects.size())
			{
				for (auto r : rects.PlateRects)
				{
					r &= image_rect;
					cv::Mat i = cv::Mat(frame, r);

					cv::Mat gray_image;
					cv::Mat test_image;
					cv::cvtColor(i, gray_image, CV_RGB2GRAY);
					cv::resize(i, test_image, cv::Size(i.cols * 10, i.rows * 10));

					int dilation_size = 1;
					cv::Mat element = getStructuringElement(cv::MORPH_ERODE,
						cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1),
						cv::Point(dilation_size, dilation_size));
					/// Apply the dilation operation
					//cv::Canny(gray_image, gray_image, 128, 255);
					//erode(gray_image, gray_image, element);

					for (int i = 0; i < gray_image.rows - window_size.height; ++i)
					{
						for (int j = 0; j < gray_image.cols - window_size.width; ++j)
						{
							cv::Mat symbol_image = cv::Mat(gray_image, cv::Rect(j, i, window_size.width, window_size.height));
							cv::Mat features = eval.Features(symbol_image);

							float weight;
							//int class_id = pred.Predict(SNNumberRecognizer::DigitsAlphabet, symbol_image, eval, weight);
							//if (class_id != -1/* && class_id == 4*/ && weight > 0.5)
							//{
							//	char text[20];
							//	sprintf_s(text, "%i", class_id);
							//	cv::putText(test_image, text, cv::Point(j * 10, i * 10), CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 250));
							//	cv::rectangle(test_image, cv::Rect(j * 10, i * 10, window_size.width * 10, window_size.height * 10), cv::Scalar(255, 0, 0));
							//}

						}
					}

					int r = 0;

					/*hs.LoadImage(gray_image);

					SNNumberRecognizer::SNSymbolRanges sr;
					
					hs.DetectSymbolRanges(sr);
					hs.DebugSymbolRanges(sr, i);
					int e = 0;*/
				}
			}

			int r = time.elapsed();
			qDebug() << r;


			if (first_frame)
			{
				first_frame = false;

			}
		}
	}

	return a.exec();
}
