
#include <QtCore/QCoreApplication>
#include <opencv2/highgui/highgui.hpp>
#include "SNPlateDetector.h"
#include <QFile>
#include <QTextStream>
#include "SNHistogramSegmentor.h"
#include <opencv2/imgproc/imgproc.hpp>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	SNPlateDetector pd;

	cv::VideoCapture capture("e:/MailCloud/video_data_base/autonumbers/russia/num.avi");

	// current video frame
	cv::Mat frame;
	SNNumberRecognizer::SNHistogramSegmentor hs;
	QFile f("cascade_2.xml");
	f.open(QIODevice::ReadOnly);

	QTextStream ts(&f);
	QString s = ts.readAll();
	pd.LoadCascade(s.toStdString());

	f.close();

	if (capture.isOpened())
	{
		bool first_frame = true;

		while (true)
		{
			if (!capture.read(frame))
				break;

			SNPlateRects rects;

			pd.Detect(frame, rects);

			cv::Rect image_rect = cv::Rect(0, 0, frame.cols - 1, frame.rows - 1);
			if (rects.PlateRects.size())
			{
				for (auto r : rects.PlateRects)
				{
					r &= image_rect;
					cv::Mat i = cv::Mat(frame, r);

					cv::Mat gray_image;

					cv::cvtColor(i, gray_image, CV_RGB2GRAY);

					hs.LoadImage(gray_image);

					SNNumberRecognizer::SNSymbolRanges sr;
					
					hs.DetectSymbolRanges(sr);
					hs.DebugSymbolRanges(sr, i);
					int e = 0;
				}
			}

			if (first_frame)
			{
				first_frame = false;

			}
		}
	}

	return a.exec();
}
