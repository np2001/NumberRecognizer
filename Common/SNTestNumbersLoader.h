#ifndef SNTestNumbersLoader_h__
#define SNTestNumbersLoader_h__
//-----------------------------------------------------------------------------
#include <QString>
#include <QRect>
#include <stdint.h>
#include <map>
#include <list>
#include <QPointF>
#include <vector>
#include "SNNumberRecognizerDefines.h"
#include "SNANNFeatureEvaluator.h"
//-----------------------------------------------------------------------------
struct SNTestSymbolDescriptor
{
	QRectF SymbolRect;
	float SymbolValidity;
	int SymbolCode;
};
//-----------------------------------------------------------------------------
typedef std::vector<SNTestSymbolDescriptor> SNTestSymbols;
//-----------------------------------------------------------------------------

struct SNTestNumberDescriptor
{
	uint64_t Timestamp;
	QString Number;
	QString NumberFormat;
	float NumberValidity;
	QRect ExactRect;
	QRect OuterRect;
	SNTestSymbols Symbols;
	float Angle;
};
//-----------------------------------------------------------------------------
typedef std::list<SNTestNumberDescriptor> SNTestNumberDescriptorList;
typedef std::map<uint64_t, SNTestNumberDescriptorList> SNTestNumbers;
//-----------------------------------------------------------------------------

class SNTestNumbersLoader
{
public:
	SNTestNumbersLoader();
	~SNTestNumbersLoader();
	void Load(SNTestNumbers& test_numbers, QString path, QString filename = "descr.txt");
	void LoadSymbols(SNNumberRecognizer::ANNAlphabets& alphabets, SNANNFeatureEvaluator& eval, QString base_path, QString alphabets_path, QString filename = "alphabets_config.xml");
private:
	void Parse(const QStringList& files, SNTestNumbers& test_numbers);
	void InitAlphabets(SNNumberRecognizer::ANNAlphabets& alphabets, QString path, QString filename = "alphabets_config.xml");
};
//-----------------------------------------------------------------------------
#endif // SNTestNumbersLoader_h__
