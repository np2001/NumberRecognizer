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

	//struct SNANNPredictionResult
	//{
	//	int32_t ClassID;
	//	float Weight;
	//	cv::Point TopLeft;
	//	cv::Size WinSize;
	//};
	////------------------------------------------------------------------------------
	//typedef std::vector<SNANNPredictionResult> SNANNPredictionResultList;
	////------------------------------------------------------------------------------

	//struct SNANNLinePredictionResults : public std::map<int32_t, SNANNPredictionResultList>
	//{
	//	float WeightSum;
	//	int32_t LineNum;
	//};
	////------------------------------------------------------------------------------
	//typedef std::vector<SNANNLinePredictionResults> SNANNPredictionResults;
	//------------------------------------------------------------------------------
	//bool SortANNPredictionResults(const SNANNLinePredictionResults& i, const SNANNLinePredictionResults& j);
	//------------------------------------------------------------------------------
	//typedef std::map<int32_t, SNANNPredictionResult> SNColumnPredictionResults;
	////------------------------------------------------------------------------------
	//struct SNPlateVariant : public SNANNPredictionResultList
	//{
	//	float WeightSum;
	//	std::string Number;
	//};
	////------------------------------------------------------------------------------
	//typedef std::vector<SNPlateVariant> SNPlateVariants;
	////------------------------------------------------------------------------------
	//typedef std::map<std::string, float> SNPlateVariantsRating;
	////------------------------------------------------------------------------------

	//bool SortANNPredictionResults(const SNANNLinePredictionResults& i, const SNANNLinePredictionResults& j);
}
//---------------------------------------------------------------------
#endif // SNNumberDefines_h__
