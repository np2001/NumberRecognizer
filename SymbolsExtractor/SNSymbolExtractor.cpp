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

void SNSymbolExtractor::Extract(const SNTestNumbers& numbers, QString in_files_path, const QString& out_path, float min_symbol_probability /*= 0.99*/, int min_symbol_height /*= 12*/)
{
	SNNumberRecognizer::ANNSymbolClassesMap symbols;

	SNNumberRecognizer::SNPlateList plates;
	

	int count = 0;
	for (auto num_file : numbers)
	{
		for (auto num : num_file.second)
		{
			QString filename = QString("%1/%2.png").arg(in_files_path).arg(num.Timestamp);

			//qDebug() << filename;

			cv::Mat image = cv::imread(filename.toLocal8Bit().data());

			if (image.cols && image.rows)
			{
				cv::Mat plate = cv::Mat(
					image,
					cv::Rect(
					num.ExactRect.x(),
					num.ExactRect.y(),
					num.ExactRect.width(),
					num.ExactRect.height()));

				/// Compute a rotation matrix with respect to the center of the image
				cv::Point center = cv::Point(plate.cols / 2, plate.rows / 2);
				//cv::Point center = cv::Point(0, 0);
				double angle = -num.Angle;
				
				/// Get the rotation matrix with the specifications above
				cv::Mat rot_mat = getRotationMatrix2D(center, angle, 1.0);

				/// Rotate the warped image
				warpAffine(plate, plate, rot_mat, plate.size());

				plates.push_back(plate.clone());

				for (auto sym : num.Symbols)
				{
					if (sym.SymbolValidity >= min_symbol_probability)
					{
						if (sym.SymbolRect.width())
						{
							if (sym.SymbolRect.left() > 0 && sym.SymbolRect.top() > 0 && sym.SymbolRect.right() < image.cols && sym.SymbolRect.bottom() < image.rows)
							{
								//cv::getRectSubPix(image, cv::Size(sym.SymbolRect.width(), sym.SymbolRect.height()), cv::Point2f(sym.SymbolRect.center().x(), sym.SymbolRect.center().y()), symbol);

								/*cv::Mat symbol = cv::Mat(
									image,
									cv::Rect(
									num.ExactRect.x() + (sym.SymbolRect.left() / 268.0 * num.ExactRect.width()),
									num.ExactRect.y() + (sym.SymbolRect.top() / 60.0 * num.ExactRect.height()),
									sym.SymbolRect.width() / 268.0 * num.ExactRect.width(),
									sym.SymbolRect.height() / 60.0 * num.ExactRect.height()));*/

								cv::Mat symbol = cv::Mat(
								plate,
								cv::Rect(
								sym.SymbolRect.left() / 268.0 * num.ExactRect.width(),
								sym.SymbolRect.top() / 60.0 * num.ExactRect.height(),
								sym.SymbolRect.width() / 268.0 * num.ExactRect.width(),
								sym.SymbolRect.height() / 60.0 * num.ExactRect.height()));
								
								if (symbol.rows >= min_symbol_height)
								{
									SNNumberRecognizer::ANNClassItem item;
									item.Image = symbol.clone();
									symbols[sym.SymbolCode].push_back(item);
								}
							}
						}
					}
				}

				
				for (int i = 0; i < 10; ++i)
				if (plate.cols > NON_SYMBOL_WIDTH && plate.rows > NON_SYMBOL_HEIGHT)
				{
					cv::Rect random_rect = cv::Rect(
						rand() % (plate.cols - NON_SYMBOL_WIDTH),
						rand() % (plate.rows - NON_SYMBOL_HEIGHT),
						NON_SYMBOL_WIDTH, NON_SYMBOL_HEIGHT);

					cv::Mat non_symbol = cv::Mat(plate, random_rect);
					bool is_not_symbol = true;

					for (auto sym : num.Symbols)
					{
						if (sym.SymbolRect.width())
						{
							if (sym.SymbolRect.left() > 0 && sym.SymbolRect.top() > 0 && sym.SymbolRect.right() < image.cols && sym.SymbolRect.bottom() < image.rows)
							{
									cv::Rect symbol_rect = cv::Rect(
									sym.SymbolRect.left() / 268.0 * num.ExactRect.width(),
									sym.SymbolRect.top() / 60.0 * num.ExactRect.height(),
									sym.SymbolRect.width() / 268.0 * num.ExactRect.width(),
									sym.SymbolRect.height() / 60.0 * num.ExactRect.height());

									cv::Rect intersect = random_rect & symbol_rect;
									if (intersect.area() * 2 > symbol_rect.area())
									{
										is_not_symbol = false;
										break;
									}
							}
						}
					}

					if (is_not_symbol)
					{
						SNNumberRecognizer::ANNClassItem item;
						item.Image = non_symbol.clone();

						symbols[NON_SYMBOL_CLASS].push_back(item);
					}
				}
			}

			count++;

			if (count % 100 == 0)
				qDebug() << QString("Processed %1/%2 files").arg(count).arg(numbers.size());
		}
	}

	SaveExtracted(out_path, symbols, plates);
}
//------------------------------------------------------------

void SNSymbolExtractor::SaveExtracted(const QString& out_path, SNNumberRecognizer::ANNSymbolClassesMap& symbols, const SNNumberRecognizer::SNPlateList& plates)
{
	qDebug() << "Saving";

	int min_count = 1000000;
	int max_count = 0;

	QString plates_dir_name = out_path + "/plates/";
	QDir plates_dir;
	plates_dir.mkpath(plates_dir_name);

	int plates_count = 1;
	for (auto& plate : plates)
	{
		QString filename = QString("%1/%2.bmp").arg(plates_dir_name).arg(plates_count++);

		cv::imwrite(filename.toLocal8Bit().data(), plate);
	}

	for (auto& sym_class : symbols)
	{
		int count = 1;

		QString dir_name = out_path + "/" + QString("%1/").arg(sym_class.first);
		QDir dir;
		dir.mkpath(dir_name);

		QFile dat_file(QString("%1/%2.dat").arg(out_path).arg(sym_class.first));
		dat_file.open(QIODevice::WriteOnly);

		QTextStream dat_ts(&dat_file);

		if (sym_class.second.size() > max_count)
		{
			max_count = sym_class.second.size();
		}

		if (sym_class.second.size() < min_count)
		{
			min_count = sym_class.second.size();
		}

		qDebug() << QString("Class %1 count = %2").arg(sym_class.first).arg(sym_class.second.size());

		for (auto& sym : sym_class.second)
		{
			int curr_count = count++;
			QString filename = QString("%1/%2.bmp").arg(dir_name).arg(curr_count);
			sym.Count = curr_count;

			cv::imwrite(filename.toLocal8Bit().data(), sym.Image);

			QString dat_file_line = QString("%1/%2.bmp 1 0 0 %3 %4\n").arg(sym_class.first).arg(curr_count).arg(sym.Image.cols).arg(sym.Image.rows);
			dat_ts << dat_file_line;
		}

		dat_file.close();
	}

	for (auto& neg_sym_class : symbols)
	{
		QFile neg_dat_file(QString("%1/neg_%2.dat").arg(out_path).arg(neg_sym_class.first));
		neg_dat_file.open(QIODevice::WriteOnly);

		QTextStream neg_dat_ts(&neg_dat_file);

		for (auto& sym_class : symbols)
		{
			if (neg_sym_class.first != sym_class.first)
			{
				for (auto& sym : sym_class.second)
				{
					//if (qrand() < RAND_MAX / 20)
					{
						QString neg_dat_file_line = QString("%1/%2.bmp\n").arg(sym_class.first).arg(sym.Count);
						neg_dat_ts << neg_dat_file_line;
					}
				}
			}
		}

		neg_dat_file.close();
	}

	qDebug() << QString("Min count = %1 MaxCount = %2").arg(min_count).arg(max_count);
}
//------------------------------------------------------------
