
#include <QtCore/QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QTime>
#include <iostream>
#include <QDebug>
#include "opencv2/highgui/highgui.hpp"
#include <stdint.h>
#include "SNNumrecInterface.h"
#include <opencv2/imgproc/imgproc.hpp>

std::map<uint64_t, cv::Mat> ImageRetainMap;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	SNNumrecInterface * nri = new SNNumrecInterface("SNNumrecDLLd.dll");

	cv::VideoCapture capture("e:/MailCloud/video_data_base/autonumbers/russia/Êטלנû.avi");

	cv::Mat frame;
	
	cv::Size window_size = cv::Size(8, 14);

	int frame_count = 0;

	uint64_t frame_id = 0;

	if (capture.isOpened())
	{
		while (true)
		{
			if (!capture.read(frame))
				break;

			//Sleep(25);

			frame_count++;

			cv::imshow("src", frame);

			SNNumberRecognizerInputFrame frame2;
			frame_id += 400000ULL;

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
				frame2.FrameID = frame_id;
				frame2.ROIY = 0.0;
				frame2.ROIWidth = 1.0;
				frame2.ROIHeight = 1.0;

				if (nri->Process(&frame2))
				{
					ImageRetainMap[frame2.FrameID] = rgb32_image.clone();
				}

				if (frame2.ResultsCount)
				{
					for (int32_t i = 0; i < frame2.ResultsCount; ++i)
					{
						auto iter = ImageRetainMap.find(frame2.Results[i].BestFrameID);
						int r = 0;
					}
				}

				if (frame2.FramesToReleaseCount)
				{
					for (int32_t i = 0; i < frame2.FramesToReleaseCount; ++i)
					{
						auto iter = ImageRetainMap.find(frame2.FramesToRelease[i]);
						if (iter != ImageRetainMap.end())
						{
							ImageRetainMap.erase(iter);
						}
						else
						{
							int r = 0;
						}
					}
				}

				int r = time.elapsed();
				qDebug() << r;
			}
		}
	}

	return a.exec();
}
