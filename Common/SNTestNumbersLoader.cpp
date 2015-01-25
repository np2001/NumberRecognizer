#include "SNTestNumbersLoader.h"
#include <QDir>
#include <QTextStream>
#include <QtXML/QDomDocument>
#include "opencv2\highgui\highgui.hpp"
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

void SNTestNumbersLoader::InitAlphabets(SNNumberRecognizer::ANNAlphabets& alphabets, QString path, QString filename /*= "alphabets_config.xml"*/)
{
	QDomDocument alphabets_data;
	QString file_path = path + "/" + filename;
	QFile file(file_path);

	if (alphabets_data.setContent(&file))
	{
		QDomElement root = alphabets_data.documentElement();
		for (QDomElement alph = root.firstChildElement("Alphabet"); !alph.isNull(); alph = alph.nextSiblingElement("Alphabet"))
		{
			SNNumberRecognizer::AlphabetTypes alphabet_id = (SNNumberRecognizer::AlphabetTypes)alph.attribute("ID").toInt();

			alphabets[alphabet_id];

			int32_t symbol_class_id = 0;

			for (QDomElement sym = alph.firstChildElement("SymbolClass"); !sym.isNull(); sym = sym.nextSiblingElement("SymbolClass"))
			{
				QString display_symbol = sym.attribute("DisplaySymbol");
				alphabets[alphabet_id].SymbolClasses[symbol_class_id].DisplaySymbol = display_symbol[0].toAscii();
				alphabets[alphabet_id].SymbolClasses[symbol_class_id].SymbolCode = sym.attribute("ID").toInt();
				alphabets[alphabet_id].SymbolClasses[symbol_class_id].clear();
				symbol_class_id++;
			}
		}
	}
}
//-----------------------------------------------------------------------------

void SNTestNumbersLoader::LoadSymbols(SNNumberRecognizer::ANNAlphabets& alphabets, SNANNFeatureEvaluator& eval, QString base_path, QString alphabets_path, QString filename /*= "alphabets_config.xml"*/)
{
	InitAlphabets(alphabets, base_path, filename);

	QString path = base_path + "/" + alphabets_path + "/";

	for (auto& alph : alphabets)
	{
		QString alphabet_path = path + "/" + QString("%1").arg(alph.first) + "/";
		for (auto& sym_class : alph.second.SymbolClasses)
		{
			QString sym_class_path = alphabet_path + "/" + QString("%1").arg(sym_class.first) + "/";

			QDir dir(sym_class_path);

			QStringList filters;
			filters << "*.bmp";
			QStringList files = dir.entryList(filters);

			for (auto f : files)
			{
				QString image_filename = sym_class_path + f;

				SNNumberRecognizer::ANNClassItem item;
				item.Image = cv::imread(image_filename.toStdString(), 0);
				item.Features = eval.Features(item.Image);
				sym_class.second.push_back(item);
			}
		}

		QString non_sym_class_path = alphabet_path + "/nonsym/";

		QDir dir(non_sym_class_path);

		QStringList filters;
		filters << "*.bmp";
		QStringList files = dir.entryList(filters);

		for (auto f : files)
		{
			QString image_filename = non_sym_class_path + f;

			SNNumberRecognizer::ANNClassItem item;
			item.Image = cv::imread(image_filename.toStdString(), 0);
			item.Features = eval.Features(item.Image);
			alph.second.NonSymbols.push_back(item);
		}
	}
}
//-----------------------------------------------------------------------------
