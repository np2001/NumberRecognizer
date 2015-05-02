#ifndef SNNumberDefines_h__
#define SNNumberDefines_h__
//---------------------------------------------------------------------
#include "opencv2\ml\ml.hpp"
#include <stdint.h>
#include <list>
#include <map>
//---------------------------------------------------------------------

namespace SNNumberRecognizer
{
	//------------------------------------------------------------------------------
	enum AlphabetTypes
	{
		DigitsAlphabet = 0,
		LettersAlphabet = 1
	};
	//------------------------------------------------------------------------------
	struct ANNClassItem
	{
		cv::Mat Features;
		cv::Mat Image;
		int32_t Count;
	};
	//------------------------------------------------------------------------------
	struct ANNSymbolClassItems : public std::list<ANNClassItem>
	{
		char DisplaySymbol;
	};
	//------------------------------------------------------------------------------
	typedef int32_t ANNClassID;
	//------------------------------------------------------------------------------
	typedef std::map<ANNClassID, ANNSymbolClassItems> ANNSymbolClassesMap;
	//------------------------------------------------------------------------------
	struct ANNAlphabet
	{
		CvANN_MLP  ANN;
		ANNSymbolClassesMap SymbolClasses;
		//ANNSymbolClassItems NonSymbols;
	};
	//------------------------------------------------------------------------------
	typedef std::map<AlphabetTypes, ANNAlphabet> ANNAlphabets;
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------

	struct SNANNConfig
	{
		std::string ConfigString;
		ANNSymbolClassesMap CharClassIDs;
	};
	//------------------------------------------------------------------------------
	typedef std::map<AlphabetTypes, SNANNConfig> SNANNConfigMap;
	//------------------------------------------------------------------------------
	typedef std::list<cv::Mat> SNPlateList;
	//------------------------------------------------------------------------------
}
//---------------------------------------------------------------------

typedef std::vector<SNNumberRecognizer::AlphabetTypes> SNNumberFormat;
//--------------------------------------------------------------------------

struct SNNumberVariant
{
	std::string Number;
	float Weight;
};
//--------------------------------------------------------------------------
typedef std::vector<SNNumberVariant> SNNumberVariants;
//--------------------------------------------------------------------------

struct SNNumberRecognizerInputFrame
{
	char* RGB32Image;
	uint32_t Width;
	uint32_t Height;
	uint64_t FrameID;
};
//--------------------------------------------------------------------------

#endif // SNNumberDefines_h__
