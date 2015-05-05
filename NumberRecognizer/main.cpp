
#include <QtCore/QCoreApplication>
#include <opencv2/highgui/highgui.hpp>
#include <QFile>
#include <QTextStream>
#include <opencv2/imgproc/imgproc.hpp>
#include <QTime>
#include <iostream>
#include "../Common/SNNumberRecognizerDefines.h"
#include <QDebug>
#include "SNRecognizer.h"

SNNumberRecognizer::SNRecognizer nr;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	cv::VideoCapture capture("e:/MailCloud/video_data_base/autonumbers/russia/кимры.avi");

	cv::Mat frame;
	
	cv::Size window_size = cv::Size(8, 14);

	int frame_count = 0;

	uint64_t frame_id = 0;

	SNNumberRecognizer::SNNumberRecognizerInputFrame frame2;
	frame2.FrameID = 0;

	if (capture.isOpened())
	{
		while (true)
		{
			if (!capture.read(frame))
				break;

			//Sleep(25);

			frame_count++;

			cv::imshow("src", frame);

			cv::waitKey(1);
			//if (frame_count > 1673/* && frame_count < 293*/)
			//if (frame_count > 100/* && frame_count < 293*/)
			{
				cv::Mat rgb32_image;
				cvtColor(frame, rgb32_image, CV_RGB2RGBA);

				QTime time;
				time.start();

				frame2.RGB32Image = (char*)rgb32_image.data;
				frame2.Width = rgb32_image.cols;
				frame2.Height = rgb32_image.rows;
				frame2.FrameID += 400000ULL;
				frame2.ROIY = 0.0;
				frame2.ROIWidth = 1.0;
				frame2.ROIHeight = 1.0;

				nr.Process(frame2);

				/*cv::resize(plate_image, plate_image, plate_image.size() * 4);

					if (!variants.empty())
					{
					cv:putText(plate_image, variants.front().Number, cv::Point(5, plate_image.rows - 5), CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0));
					}

					int r = 0;*/

				int r = time.elapsed();
				qDebug() << r;
			}
		}
	}

	return a.exec();
}
