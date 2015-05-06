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

	struct SNPlate
	{
		// оординаты пластины на полном изображении
		cv::Rect GlobalRect;
		// оординаты пластины на вырезанном изображении
		cv::Rect LocalRect;

		uint64_t FrameID;
		cv::Mat PlateImage;

		SNFigureGroups FigureGroups;
	};
	//------------------------------------------------------------------------------------

	struct SNPlates : public std::vector<SNPlate>
	{
		
	};
	//------------------------------------------------------------------------------------


	struct ANNPredictionResult
	{
		char Symbol;
		float Weight;
	};

	struct ANNPredictionResults : public std::list <ANNPredictionResult>
	{
		void Sort()
		{
			sort([](const ANNPredictionResult & a, const ANNPredictionResult & b) -> bool
			{
				return a.Weight > b.Weight;
			});
		}
	};

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
	typedef std::list<cv::Mat> SNPlateList;
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

	struct SNNumberRecognizerInputFrame
	{
		char* RGB32Image;
		uint32_t Width;
		uint32_t Height;
		uint64_t FrameID;
		float ROIX;
		float ROIY;
		float ROIWidth;
		float ROIHeight;

		SNNumberRecognizerInputFrame()
		{
			ROIX = 0.0f;
			ROIY = 0.0f;
			ROIWidth = 1.0f;
			ROIHeight = 1.0f;
		}
	};

	struct SNSymbolStats
	{
		ANNPredictionResults DigitsStats;
		ANNPredictionResults LetterStats;
	};
	//------------------------------------------------------
	struct SNNumberStats : public std::vector < SNSymbolStats >
	{
		uint64_t FrameID;
		cv::Rect PlateRect;
		cv::Mat Plate;
	};
	//------------------------------------------------------
	struct SNNumberStatsGroup : public std::vector < SNNumberStats >
	{
		uint64_t BestFrameID;
		uint64_t LastFrameID;

		cv::Rect BestPlateRect;
		cv::Rect LastRect;
		cv::Mat Plate;
	};
	//------------------------------------------------------
	typedef std::vector<SNNumberStatsGroup> SNNumberStatsGroups;
	//------------------------------------------------------
	//--------------------------------------------------------------------------
}
#endif // SNNumberDefines_h__
