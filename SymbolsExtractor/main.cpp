
#include <QtCore/QCoreApplication>
#include "SNSymbolExtractor.h"
#include "../Common/SNTestNumbersLoader.h"
#include "opencv2/highgui/highgui.hpp"
#include <windows.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QString base_path = "e:/test_images";
	QString out_path = "e:/symbols6";

	
	SNSymbolExtractor SE;
	SNTestNumbersLoader TNL;

	SNTestNumbers tn;

	TNL.Load(tn, base_path);

	//TNL.WarpSymbols(tn, 0.9, 0.9);

	SE.Extract(tn, base_path, out_path, 0.999);

	return a.exec();
}
