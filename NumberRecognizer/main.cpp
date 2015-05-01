
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
#include "SNPlateRecognizer.h"
#include <windows.h>

SNNumberRecognizer::SNPlateRecognizer pr;

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

	QFile f("../ANNTrainer/best.ann");
	f.open(QIODevice::ReadOnly);

	QByteArray cfg = f.readAll();
	pr.InitRecognizer(cfg.data());
	f.close();

	srand(QDateTime::currentDateTime().time().msec());

	/*while (true)
	{
	int i = rand() % 1600;
	char c[1000];
	sprintf_s(c, sizeof(c), "e:/symbols7/plates/%i.bmp", i);

	cv::Mat image = cv::imread(c, cv::IMREAD_GRAYSCALE);

	if (image.size().area())
	{

	cv::resize(image, image, image.size() * 1);

	SNNumberVariants variants;

	QTime time;
	time.start();

	SNFigureGroups fgs;
	pr.RecognizePlate(image, fgs, variants);

	cv::Mat debug_image;
	pr.DebugFigureGroups(image, fgs, debug_image, 2);

	assert(image.size().area());

	cv::resize(image, image, image.size() * 4);

	if (!variants.empty())
	{
	cv:putText(image, variants.front().Number, cv::Point(5, image.rows - 5), CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0));
	}

	int t = time.elapsed();

	qDebug() << t;
	}
	}


	getch();*/

	LoadCascade(pd, "../cascade_2.xml");
	
	cv::VideoCapture capture("e:/MailCloud/video_data_base/autonumbers/russia/Êטלנû.avi");

	cv::Mat frame;
	
	cv::Size window_size = cv::Size(8, 14);

	int frame_count = 0;

	if (capture.isOpened())
	{
		while (true)
		{
			if (!capture.read(frame))
				break;

			Sleep(25);

			frame_count++;
			
			//if (frame_count > 1673/* && frame_count < 293*/)
			//if (frame_count > 390/* && frame_count < 293*/)
			{
				SNPlateRects rects;

				QTime time;
				time.start();

				pd.Detect(frame, rects);

				cv::Rect image_rect = cv::Rect(0, 0, frame.cols - 1, frame.rows - 1);

				if (rects.PlateRects.size())
				{
					for (auto r : rects.PlateRects)
					{
						r &= image_rect;
						cv::Mat plate_image = cv::Mat(frame, r);

						cv::Mat gray_image;
						//cv::Mat test_image;
						cv::cvtColor(plate_image, gray_image, CV_RGB2GRAY);
						//cv::resize(plate_image, test_image, cv::Size(plate_image.cols * 10, plate_image.rows * 10));

						QTime time;
						time.start();

						SNFigureGroups fgs;
						SNNumberVariants variants;
						pr.RecognizePlate(gray_image, fgs, variants);

						cv::Mat debug_image;
						pr.DebugFigureGroups(gray_image, fgs, debug_image, 2);

						cv::resize(plate_image, plate_image, plate_image.size() * 4);

						if (!variants.empty())
						{
						cv:putText(plate_image, variants.front().Number, cv::Point(5, plate_image.rows - 5), CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0));
						}

						int r = 0;
					}
				}

				int r = time.elapsed();
				qDebug() << r;
			}
			
		}
	}

	return a.exec();
}
