#ifndef SNNumberDefines_h__
#define SNNumberDefines_h__
//---------------------------------------------------------------------
#include "opencv2\ml\ml.hpp"
#include <stdint.h>
#include <list>
#include <map>
#include "..\NumberRecognizer\SNMasterRecognizerStructs.h"
//---------------------------------------------------------------------

namespace SNNumberRecognizer
{
	//------------------------------------------------------------------------------
	struct ANNPredictionResult
	{
		char Symbol;
		float Weight;
	};

	struct ANNPredictionResults : public std::list < ANNPredictionResult >
	{
		void Sort()
		{
			sort([](const ANNPredictionResult & a, const ANNPredictionResult & b) -> bool
			{
				return a.Weight > b.Weight;
			});
		}
	};
	//------------------------------------------------------------------------------
	
	struct SNSymbolStats
	{
		ANNPredictionResults DigitsStats;
		ANNPredictionResults LetterStats;
	};
	//------------------------------------------------------
	struct SNNumberStats : public std::vector<SNSymbolStats>
	{
		float TotalWeight;

		SNNumberStats()
		{
			TotalWeight = 0;
		}
	};
	//------------------------------------------------------
	
	struct SNPlate
	{
		// оординаты пластины на полном изображении
		cv::Rect GlobalRect;
		// оординаты пластины на вырезанном изображении
		cv::Rect LocalRect;

		uint64_t FrameID;
		cv::Mat PlateImage;

		SNFigureGroups FigureGroups;
		SNNumberStats Stats;
	};
	//------------------------------------------------------------------------------------

	struct SNNumberStatsGroup : public std::vector < SNPlate >
	{
		uint64_t BestFrameID;
		float BestWeight;
		uint64_t LastFrameID;

		cv::Rect BestPlateRect;
		cv::Rect LastRect;
		cv::Mat Plate;
	};
	//------------------------------------------------------
	typedef std::vector<SNNumberStatsGroup> SNNumberStatsGroups;
	//------------------------------------------------------

	struct SNPlates : public std::list<SNPlate>
	{
		
	};
	//------------------------------------------------------------------------------------

	enum AlphabetTypes
	{
		UnknownAlphabet = -1,
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
	struct ANNSymbolClassItems : public std::list < ANNClassItem >
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
	//typedef std::list<cv::Mat> SNPlateList;
	//------------------------------------------------------------------------------

	struct SNNumberVariant
	{
		std::string Number;
		float Weight;
	};
	//--------------------------------------------------------------------------
	typedef std::vector<SNNumberVariant> SNNumberVariants;
	//--------------------------------------------------------------------------

	struct SNPlateModelSymbol
	{
		cv::Rect SymbolRect;
		AlphabetTypes SymbolType;
		SNFigureGroup SymbolVariants;
	};
	//------------------------------------------------------
	typedef std::vector<SNPlateModelSymbol> SNPlateModel;
	typedef std::vector<SNPlateModel> SNPlateModels;
	//------------------------------------------------------

	struct SNPlateModelCenterVector
	{
		cv::Point2f Vector;
		float VectorLength;
	};
	//--------------------------------------------------------------------------
	typedef std::vector<SNPlateModelCenterVector> SNPlateModelCenterVectors;
	//--------------------------------------------------------------------------

	typedef std::list<uint64_t> SNFramesToRelease;
	//--------------------------------------------------------------------------
	struct SNFinalResult
	{
		uint64_t BestFrameID;
		cv::Rect BestPlateRect;
		cv::Mat BestPlate;
		std::string Number;
		float Weight;
	};
	//--------------------------------------------------------------------------
	typedef std::list<SNFinalResult> SNFinalResults;
	//--------------------------------------------------------------------------

	struct SNRecognizerProcessorQuery
	{
		SNPlate PlateToRecognize;
		SNFramesToRelease FramesToRelease;
		uint64_t LastFrameID;
		SNFinalResults FinalResults;
	};
	//--------------------------------------------------------------------------

}
#endif // SNNumberDefines_h__
