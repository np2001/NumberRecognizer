#include "SNSymbolExtractor.h"
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "opencv2\imgproc\imgproc.hpp"
#include <QDebug>
//#include "SNExactRectDetector.h"
//------------------------------------------------------------
SNSymbolExtractor::SNSymbolExtractor()
{
}
//------------------------------------------------------------

SNSymbolExtractor::~SNSymbolExtractor()
{
}
//------------------------------------------------------------

void SNSymbolExtractor::Extract(const SNTestNumbers& numbers, QString in_files_path, const QString& out_path, float min_symbol_probability /*= 0.99*/)
{
	SNNumberRecognizer::ANNSymbolClassesMap symbols;

	int count = 0;
	for (auto num_file : numbers)
	{
		for (auto num : num_file.second)
		{
			QString filename = QString("%1/%2.png").arg(in_files_path).arg(num.Timestamp);

			//qDebug() << filename;

			cv::Mat image = cv::imread(filename.toLocal8Bit().data());

			if (image.cols)
			{
				for (auto sym : num.Symbols)
				{
					if (sym.SymbolValidity >= min_symbol_probability)
					{
						if (sym.SymbolRect.width())
						{
							if (sym.SymbolRect.left() > 0 && sym.SymbolRect.top() > 0 && sym.SymbolRect.right() < image.cols && sym.SymbolRect.bottom() < image.rows)
							{
								//cv::getRectSubPix(image, cv::Size(sym.SymbolRect.width(), sym.SymbolRect.height()), cv::Point2f(sym.SymbolRect.center().x(), sym.SymbolRect.center().y()), symbol);

								cv::Mat symbol = cv::Mat(
									image,
									cv::Rect(
									num.ExactRect.x() + (sym.SymbolRect.left() / 268.0 * num.ExactRect.width()),
									num.ExactRect.y() + (sym.SymbolRect.top() / 60.0 * num.ExactRect.height()),
									sym.SymbolRect.width() / 268.0 * num.ExactRect.width(),
									sym.SymbolRect.height() / 60.0 * num.ExactRect.height()));

								SNNumberRecognizer::ANNClassItem item;
								item.Image = symbol.clone();
								symbols[sym.SymbolCode].push_back(item);
							}
						}
					}
				}
			}

			count++;

			if (count % 100 == 0)
				qDebug() << QString("Processed %1/%2 files").arg(count).arg(numbers.size());
		}
	}

	SaveExtracted(out_path, symbols);
}
//------------------------------------------------------------

//void SNSymbolExtractor::ExtractPlate(const SNTestNumbers& numbers, QString in_files_path, QString out_files_path, SNPlateModel model, float min_plate_probability /*= 50.0*/)
//{
//	int count = 1;

//	for (auto num_file : numbers)
//	{
//		for (auto num : num_file.second)
//		{
//			if (num.NumberValidity >= min_plate_probability && num.NumberFormat == "zXXXzzxx")
//			{
//				QString filename = QString("%1/%2.png").arg(in_files_path).arg(num.Timestamp);
//				cv::Mat image = cv::imread(filename.toLocal8Bit().data());

//				QString out_filename = QString("%1/%2.bmp").arg(out_files_path).arg(count++);

//				cv::Mat plate = cv::Mat(image, cv::Rect(num.ExactRect.left(), num.ExactRect.top(), num.ExactRect.width(), num.ExactRect.height()));

//				cv::resize(plate, plate, cv::Size(plate.cols * 10, plate.rows * 10));

//				for (auto sym : model)
//				{
//					cv::circle(plate, cv::Point2f(sym.Center.x * plate.cols, sym.Center.y * plate.rows), 3, cv::Scalar(0, 0, 255));
//					cv::rectangle(plate, cv::Rect((sym.Center.x - sym.MeanSize.x / 2) * plate.cols, (sym.Center.y - sym.MeanSize.y / 2) * plate.rows, sym.MeanSize.x * plate.cols, sym.MeanSize.y * plate.rows), cv::Scalar(0, 0, 255));
//				}

//				cv::imwrite(out_filename.toLocal8Bit().data(), plate);
//			}
//		}
//	}
//}
////------------------------------------------------------------

void SNSymbolExtractor::SaveExtracted(const QString& out_path, const SNNumberRecognizer::ANNSymbolClassesMap& symbols)
{
	qDebug() << "Saving";

	for (auto& sym_class : symbols)
	{
		int count = 1;

		QString dir_name = out_path + "/" + QString("%1/").arg(sym_class.first);
		QDir dir;
		dir.mkpath(dir_name);

		for (auto& sym : sym_class.second)
		{
			QString filename = QString("%1/%2.bmp").arg(dir_name).arg(count++);

			cv::imwrite(filename.toLocal8Bit().data(), sym.Image);
		}
	}
}
//------------------------------------------------------------
