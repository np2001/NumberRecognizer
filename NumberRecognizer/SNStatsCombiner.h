#ifndef SNStatsCombiner_h__
#define SNStatsCombiner_h__
//-----------------------------------------------------------------
#include "SNMasterRecognizerStructs.h"
#include "SNFormatMatcher.h"
//-----------------------------------------------------------------
namespace SNNumberRecognizer
{
	class SNStatsCombiner
	{
	public:
		SNStatsCombiner();
		~SNStatsCombiner();
		void CombineStats(const SNNumberStats& stats);
		void CheckResults(const uint64_t frame_id);
	private:
		SNNumberStatsGroups NumberStatsGroups;
		SNFormatMatcher FormatMatcher;
	private:
		void CombineStats(const SNNumberStats& src, SNNumberStats& dst);
		void CombinePredictionResults(const ANNPredictionResults& src, ANNPredictionResults& dst);
		bool IsGroupMatched(const SNNumberStatsGroup& group, const SNNumberStats& src);
		void OutputIntermediateResults(const SNNumberStatsGroup& group);
		void DetectFinalResult(const SNNumberStatsGroup& group, SNNumberVariants& variants);
	};
};
//-----------------------------------------------------------------
#endif // SNStatsCombiner_h__
