
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

	LoadCascade(pd, "cascade_2.xml");

	SNNumberRecognizer::SNANNConfigLoader config_loader;

	SNNumberRecognizer::SNANNConfigMap test_config;
	QFile f("../ANNTrainer/test.ann");
	f.open(QIODevice::ReadOnly);

	QByteArray cfg = f.readAll();
	config_loader.Load(cfg.data(), test_config);
	f.close();

	SNNumberRecognizer::SNANNPredictor pred;
	int32_t total_count = 0;
	int32_t total_correct_count = 0;
	int32_t total_incorrect_count = 0;
	int32_t total_unrecognized_count = 0;

	pred.Load(test_config);
	
	cv::VideoCapture capture("e:/MailCloud/video_data_base/autonumbers/russia/num.avi");

	// current video frame
	cv::Mat frame;
	SNNumberRecognizer::SNHistogramSegmentor hs;
	SNANNFeatureEvaluator eval;

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

					for (int i = 0; i < gray_image.rows - window_size.height; ++i)
					{
						for (int j = 0; j < gray_image.cols - window_size.width; ++j)
						{
							cv::Mat symbol_image = cv::Mat(gray_image, cv::Rect(j, i, window_size.width, window_size.height));
							cv::Mat features = eval.Features(symbol_image);

							float weight;
							int class_id = pred.Predict(SNNumberRecognizer::DigitsAlphabet, symbol_image, eval, weight);
							if (class_id != -1 && /*class_id == 4 &&*/ weight > 0.9)
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
