#ifndef SNSymbolExtractor_h__
#define SNSymbolExtractor_h__
//------------------------------------------------------------
#include "../Common/SNTestNumbersLoader.h"
#include <QDir>
#include <map>
#include "../Common/SNNumberRecognizerDefines.h"
//------------------------------------------------------------
#define NON_SYMBOL_CLASS -1
#define NON_SYMBOL_WIDTH 8
#define NON_SYMBOL_HEIGHT 16
//------------------------------------------------------------

class SNSymbolExtractor
{
public:
	SNSymbolExtractor();
	~SNSymbolExtractor();
	void Extract(const SNTestNumbers& numbers, QString in_files_path, const QString& out_path, float min_symbol_probability = 0.99, int min_symbol_height = 12);
private:
	void SaveExtracted(const QString& out_path, SNNumberRecognizer::ANNSymbolClassesMap& symbols, const SNNumberRecognizer::SNPlateList& plates);
};
//------------------------------------------------------------
#endif // SNSymbolExtractor_h__
