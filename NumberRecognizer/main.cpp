
#include <QtCore/QCoreApplication>
#include <opencv2/highgui/highgui.hpp>
#include "SNPlateDetector.h"
#include <QFile>
#include <QTextStream>
#include "SNHistogramSegmentor.h"
#include <opencv2/imgproc/imgproc.hpp>
#include "SNDigitsComparator.h"
#include <QTime>
#include <iostream>
#include <conio.h>
#include "../Common/SNANNConfigLoader.h"
#include "../Common/SNANNPredictor.h"
#include "../Common/SNNumberRecognizerDefines.h"
#include <QDebug>
#include "SNMasterRecognizer.h"
#include "SNMasterSegmentor.h"
#include "SNMasterSymbolRecognizer.h"

SNMasterRecognizer mr;
SNMasterSegmentor ms;
SNMasterSymbolRecognizer msr;

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

	SNNumberRecognizer::SNANNConfigLoader config_loader;

	SNNumberRecognizer::SNANNConfigMap test_config;
	QFile f("../ANNTrainer/test.ann");
	f.open(QIODevice::ReadOnly);

	QByteArray cfg = f.readAll();
	config_loader.Load(cfg.data(), test_config);
	f.close();

	SNNumberRecognizer::SNANNPredictor pred;
	pred.Load(test_config);

	cv::Mat image = cv::imread("e:/symbols6/plates/117.bmp", cv::IMREAD_GRAYSCALE);
	cv::resize(image, image, image.size() * 1);

	SNFigureGroups fgs;

	QTime time;
	time.start();
	ms.Segment(image, fgs, 0, 255, 5);
	
	cv::Mat debug_image;
	ms.DebugFigureGroups(image, fgs, debug_image, 2);

	SNNumberStats stats;
	//msr.Process(image, fgs, stats);

	for (auto& fg : fgs)
	{
		stats.push_back(SNSymbolStats());

		for (auto& f : fg)
		{
			cv::Mat symbol = cv::Mat(image, cv::Rect(f.left(), f.top(), f.Width(), f.Height())).clone();

			float digit_weight;
			int digit_class_id = pred.Predict(SNNumberRecognizer::DigitsAlphabet, symbol, eval, digit_weight);

			char digit_char;
			switch (digit_class_id)
			{
			case 0: digit_char = '0'; break;
			case 1: digit_char = '1'; break;
			case 2: digit_char = '2'; break;
			case 3: digit_char = '3'; break;
			case 4: digit_char = '4'; break;
			case 5: digit_char = '5'; break;
			case 6: digit_char = '6'; break;
			case 7: digit_char = '7'; break;
			case 8: digit_char = '8'; break;
			case 9: digit_char = '9'; break;

			default:
				digit_char = '?';
				break;
			}
			
			stats.back()[digit_char] += digit_weight;

			float char_weight;
			int char_class_id = pred.Predict(SNNumberRecognizer::LettersAlphabet, symbol, eval, char_weight);

			char letter_char;
			switch (char_class_id)
			{
			case 0: letter_char = 'A'; break;
			case 1: letter_char = 'B'; break;
			case 2: letter_char = 'E'; break;
			case 3: letter_char = 'K'; break;
			case 4: letter_char = 'M'; break;
			case 5: letter_char = 'H'; break;
			case 6: letter_char = 'O'; break;
			case 7: letter_char = 'P'; break;
			case 8: letter_char = 'C'; break;
			case 9: letter_char = 'T'; break;
			case 10: letter_char = 'Y'; break;
			case 11: letter_char = 'X'; break;
			default:
				letter_char = '?';
				break;
			}

			stats.back()[letter_char] += char_weight;

			//std::pair<char, double> res = ProcessChar(symbol);
			//std::pair<char, double> res2 = ProcessNum(symbol);
			//stats.back()[res.first] += res.second;
			//stats.back()[res2.first] += res2.second;
		}
	}

	int t = time.elapsed();

	qDebug() << t;

	getch();

	LoadCascade(pd, "cascade_2.xml");

	int32_t total_count = 0;
	int32_t total_correct_count = 0;
	int32_t total_incorrect_count = 0;
	int32_t total_unrecognized_count = 0;


	cv::VideoCapture capture("e:/MailCloud/video_data_base/autonumbers/russia/num.avi");

	// current video frame
	cv::Mat frame;
	SNNumberRecognizer::SNHistogramSegmentor hs;
	
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
							int class_id = pred.Predict(SNNumberRecognizer::DigitsAlphabet, symbol_image, eval, weight);
							if (class_id != -1/* && class_id == 4*/ && weight > 0.5)
							{
								char text[20];
								sprintf_s(text, "%i", class_id);
								cv::putText(test_image, text, cv::Point(j * 10, i * 10), CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 250));
								cv::rectangle(test_image, cv::Rect(j * 10, i * 10, window_size.width * 10, window_size.height * 10), cv::Scalar(255, 0, 0));
							}

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
