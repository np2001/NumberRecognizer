#ifndef SNSymbolExtractor_h__
#define SNSymbolExtractor_h__
//------------------------------------------------------------
#include "SNTestNumbersLoader.h"
#include <QDir>
#include <map>
#include "SNNumberRecognizerDefines.h"
//------------------------------------------------------------
class SNSymbolExtractor
{
public:
	SNSymbolExtractor();
	~SNSymbolExtractor();
	void Extract(const SNTestNumbers& numbers, QString in_files_path, const QString& out_path, float min_symbol_probability = 0.99);
	//void ExtractPlate(const SNTestNumbers& numbers, QString in_files_path, QString out_files_path, SNPlateModel model, float min_plate_probability = 50.0);
	//void ExtractPlateByOuterRect(const SNNumberRecognizer::SNTestNumbers& numbers, QString in_files_path, QString out_files_path, float min_plate_probability = 50.0);
	void SaveExtracted(const QString& out_path, const SNNumberRecognizer::ANNSymbolClassesMap& symbols);
};
//------------------------------------------------------------
#endif // SNSymbolExtractor_h__
