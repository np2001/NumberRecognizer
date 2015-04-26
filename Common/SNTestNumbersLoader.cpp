#include "SNTestNumbersLoader.h"
#include <QDir>
#include <QTextStream>
#include <QtXML/QDomDocument>
#include "opencv2\highgui\highgui.hpp"
#include "QDebug"
#include "SNLocalContraster.h"
//-----------------------------------------------------------------------------
SNTestNumbersLoader::SNTestNumbersLoader()
{
}
//-----------------------------------------------------------------------------

SNTestNumbersLoader::~SNTestNumbersLoader()
{
}
//-----------------------------------------------------------------------------

void SNTestNumbersLoader::Load(SNTestNumbers& test_numbers, QString path, QString filename /*= "descr.txt"*/)
{
	QDir dir(path);
	QString descr_filename = dir.absolutePath() + "/" + filename;
	QFile file(descr_filename);
	QTextStream ts(&file);

	file.open(QIODevice::ReadOnly);

	QStringList files = ts.readAll().split("\r\n");

	file.close();

	Parse(files, test_numbers);
}
//-----------------------------------------------------------------------------

void SNTestNumbersLoader::WarpSymbols(SNTestNumbers& numbers, double offset_warp, double scale_warp)
{
	for (auto& num_file : numbers)
	{
		for (auto& num : num_file.second)
		{
			for (auto& sym : num.Symbols)
			{
				double max_scale = sym.SymbolRect.width() * scale_warp;
				double rx = (qrand() % 100) / 100.0 - 0.0;
				double ry = (qrand() % 100) / 100.0 - 0.0;
				sym.SymbolRect.adjust(0, 0, rx * max_scale, ry * max_scale);
			}
		}
	}
}
//-----------------------------------------------------------------------------

void SNTestNumbersLoader::Parse(const QStringList& files, SNTestNumbers& test_numbers)
{
	for (auto f : files)
	{
		QStringList tags = f.split(" ");
		if (tags.size() >= 9)
		{
			SNTestNumberDescriptor tnd;

			for (auto tag : tags)
			{
				QStringList tag_name_and_value = tag.split(":");
				if (tag_name_and_value.size() == 2)
				{
					QString tag_name = tag_name_and_value[0];
					QString tag_value = tag_name_and_value[1];

					if (tag_name == "TS")
					{
						tnd.Timestamp = tag_value.toULongLong();
						continue;
					}

					if (tag_name == "NUM")
					{
						tnd.Number = tag_value;
						continue;
					}

					if (tag_name == "TEMPL")
					{
						tnd.NumberFormat = tag_value.remove('|');
						continue;
					}

					if (tag_name == "NUM_VAL")
					{
						tnd.NumberValidity = tag_value.toFloat();
						continue;
					}

					if (tag_name == "OUTER_LEFT")
					{
						tnd.OuterRect.setLeft(tag_value.toInt());
						continue;
					}

					if (tag_name == "OUTER_TOP")
					{
						tnd.OuterRect.setTop(tag_value.toInt());
						continue;
					}

					if (tag_name == "OUTER_RIGHT")
					{
						tnd.OuterRect.setRight(tag_value.toInt());
						continue;
					}

					if (tag_name == "OUTER_BOTTOM")
					{
						tnd.OuterRect.setBottom(tag_value.toInt());
						continue;
					}

					if (tag_name == "EXACT_LEFT")
					{
						tnd.ExactRect.setLeft(tag_value.toInt());
						continue;
					}

					if (tag_name == "EXACT_TOP")
					{
						tnd.ExactRect.setTop(tag_value.toInt());
						continue;
					}

					if (tag_name == "EXACT_RIGHT")
					{
						tnd.ExactRect.setRight(tag_value.toInt());
						continue;
					}

					if (tag_name == "EXACT_BOTTOM")
					{
						tnd.ExactRect.setBottom(tag_value.toInt());
						continue;
					}

					if (tag_name == "ANGLE")
					{
						tnd.Angle = tag_value.toFloat();
						continue;
					}

					if (tag_name == "SYM_LEFT")
					{
						SNTestSymbolDescriptor sym;
						sym.SymbolRect.setLeft(tag_value.toInt());
						tnd.Symbols.push_back(sym);
						continue;
					}

					if (tag_name == "SYM_TOP")
					{
						tnd.Symbols.back().SymbolRect.setTop(tag_value.toInt());
						continue;
					}

					if (tag_name == "SYM_RIGHT")
					{
						tnd.Symbols.back().SymbolRect.setRight(tag_value.toInt());
						continue;
					}

					if (tag_name == "SYM_BOTTOM")
					{
						tnd.Symbols.back().SymbolRect.setBottom(tag_value.toInt());
						continue;
					}

					if (tag_name == "SYM_VALIDITY")
					{
						tnd.Symbols.back().SymbolValidity = tag_value.toFloat();
						continue;
					}

					if (tag_name == "SYM_CODE")
					{
						tnd.Symbols.back().SymbolCode = tag_value.toInt();
						continue;
					}
				}
			}

			test_numbers[tnd.Timestamp].push_back(tnd);
		}
	}
}
//-----------------------------------------------------------------------------

void SNTestNumbersLoader::LoadSymbols(SNNumberRecognizer::ANNAlphabets& alphabets, const QString& alph_config_filename)
{
	SNLocalContraster lc;
	QDomDocument alphabets_data;

	qDebug() << "Loading symbols";

	QFile file(alph_config_filename);

	if (alphabets_data.setContent(&file))
	{
		QDomElement root = alphabets_data.documentElement();

		QString alph_path = root.attribute("AlphPath");

		for (QDomElement alph = root.firstChildElement("Alphabet"); !alph.isNull(); alph = alph.nextSiblingElement("Alphabet"))
		{
			SNNumberRecognizer::AlphabetTypes alphabet_id = (SNNumberRecognizer::AlphabetTypes)alph.attribute("ID").toInt();

			alphabets[alphabet_id];

			for (QDomElement sym = alph.firstChildElement("SymbolClass"); !sym.isNull(); sym = sym.nextSiblingElement("SymbolClass"))
			{
				int32_t symbol_class_id = sym.attribute("ClassID").toInt();
				QString display_symbol = sym.attribute("DisplaySymbol");

				alphabets[alphabet_id].SymbolClasses[symbol_class_id].DisplaySymbol = display_symbol[0].toAscii();
				//alphabets[alphabet_id].SymbolClasses[symbol_class_id].SymbolCode = sym.attribute("ID").toInt();
				alphabets[alphabet_id].SymbolClasses[symbol_class_id].clear();

				QString symbol_path = alph_path + "/" + sym.attribute("SymbolPath") + "/";

				QDir dir(symbol_path);

				QStringList filters;
				filters << "*.bmp";
				QStringList files = dir.entryList(filters);

				for (auto f : files)
				{
					QString image_filename = symbol_path + f;

					SNNumberRecognizer::ANNClassItem item;
					std::string s = std::string(image_filename.toLocal8Bit().data());
					item.Image = cv::imread(s, cv::IMREAD_GRAYSCALE);
					alphabets[alphabet_id].SymbolClasses[symbol_class_id].push_back(item);
				}

				qDebug() << QString("Alphabet %1 Symbol %2 loaded %3 items").arg(alphabet_id).arg(display_symbol).arg(alphabets[alphabet_id].SymbolClasses[symbol_class_id].size());

				symbol_class_id++;
			}
		}
	}

	qDebug() << "Load completed";
}
//-----------------------------------------------------------------------------

void SNTestNumbersLoader::EvaluateFeatures(SNNumberRecognizer::ANNAlphabets& alphabets, SNANNFeatureEvaluator& eval)
{
	qDebug() << "Evaluating features";

	for (auto& alph : alphabets)
	{
		for (auto& sym_class : alph.second.SymbolClasses)
		{
			for (auto& sym : sym_class.second)
			{
				sym.Features = eval.Features(sym.Image);
			}

			qDebug() << QString("Alphabet %1 Symbol %2 Evaluated").arg(alph.first).arg(sym_class.second.DisplaySymbol);
		}
	}

	qDebug() << "Evaluation completed";
}
//-----------------------------------------------------------------------------
