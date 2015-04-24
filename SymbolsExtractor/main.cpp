
#include <QtCore/QCoreApplication>
#include "SNSymbolExtractor.h"
#include "../Common/SNTestNumbersLoader.h"
#include "opencv2/highgui/highgui.hpp"
#include "SNLocalContraster.h"
#include <windows.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QString base_path = "e:/test_images_10_12_2014";
	QString out_path = "e:/symbols6";

	while (true)
	{
		QString plate_fn;
		plate_fn = QString("f:\\symbols5\\plates\\%1.bmp").arg(GetTickCount() % 1600);

		//std::string s = plate_fn.toStdString();
		cv::Mat plate = cv::imread(plate_fn.toLocal8Bit().data(), cv::IMREAD_GRAYSCALE);

		SNLocalContraster lc;
		cv::Mat res = lc.Process(plate);
		int r = 0;
	}

	SNSymbolExtractor SE;
	SNTestNumbersLoader TNL;

	SNTestNumbers tn;

	TNL.Load(tn, base_path);

	//TNL.WarpSymbols(tn, 0.9, 0.9);

	SE.Extract(tn, base_path, out_path, 0.999);

	return a.exec();
}
