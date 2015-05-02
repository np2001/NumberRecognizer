#include "SNStatsCombiner.h"
#include <windows.h>
//-----------------------------------------------------------------
namespace SNNumberRecognizer
{
	SNStatsCombiner::SNStatsCombiner()
	{
	}
	//-----------------------------------------------------------------

	SNStatsCombiner::~SNStatsCombiner()
	{
	}
	//-----------------------------------------------------------------

	void SNStatsCombiner::CombineStats(const SNNumberStats& stats)
	{
		bool matched_group_found = false;
		for (auto& nggs : NumberStatsGroups)
		{
			if (IsGroupMatched(nggs, stats))
			{
				nggs.push_back(stats);
				matched_group_found = true;
				nggs.LastFrameID = stats.FrameID;
				nggs.LastRect = stats.PlateRect;
				nggs.Plate = stats.Plate;
				break;
			}
		}
		
		if (!matched_group_found)
		{
			SNNumberStatsGroup new_group;
			new_group.BestFrameID = stats.FrameID;
			new_group.LastFrameID = stats.FrameID;
			new_group.BestPlateRect = stats.PlateRect;
			new_group.LastRect = stats.PlateRect;
			new_group.Plate = stats.Plate;

			new_group.push_back(stats);
			NumberStatsGroups.push_back(new_group);
		}
	}
	//-----------------------------------------------------------------

	void SNStatsCombiner::CombineStats(const SNNumberStats& src, SNNumberStats& dst)
	{
		for (int i = 0; i < src.size(); ++i)
		{
			if (i < dst.size())
			{
				CombinePredictionResults(src[i].DigitsStats, dst[i].DigitsStats);
				CombinePredictionResults(src[i].LetterStats, dst[i].LetterStats);
			}
		}
		
		/*SNNumberVariants variants;
		FormatMatcher.MatchNumbers(dst, variants);*/
	}
	//-----------------------------------------------------------------

	bool SNStatsCombiner::IsGroupMatched(const SNNumberStatsGroup& group, const SNNumberStats& src)
	{
		bool res = false;
		float dx = abs(group.LastRect.x - src.PlateRect.x);
		if (dx < 120)
		{
			res = true;
		}

		if (group.size() > 1)
		{
			int sign_y1 = (group.LastRect.y - src.PlateRect.y) ? 1 : -1;
			int sign_y2 = (group.front().PlateRect.y - group.back().PlateRect.y) ? 1 : -1;
			if (sign_y1 != sign_y2)
			{
				res = false;
			}
		}

		return res;
	}

	void SNStatsCombiner::CheckResults(const uint64_t frame_id)
	{
		for (auto g = NumberStatsGroups.begin(); g != NumberStatsGroups.end();)
		{
			if (frame_id - g->LastFrameID > 5000000)
			{
				OutputIntermediateResults(*g);
				
				SNNumberVariants variants;
				DetectFinalResult(*g, variants);
				g = NumberStatsGroups.erase(g);
			}
			else
			{
				g++;
			}
		}
	}

	void SNStatsCombiner::OutputIntermediateResults(const SNNumberStatsGroup& group)
	{
		OutputDebugStringA("Intermediate results\r\n");

		for (auto& num : group)
		{
			SNNumberVariants variants;
			FormatMatcher.MatchNumbers(num, variants);

			for (auto& v : variants)
			{
				OutputDebugStringA(v.Number.c_str());
				OutputDebugStringA("\r\n");
			}
		}
	}
	//-----------------------------------------------------------------

	void SNStatsCombiner::DetectFinalResult(const SNNumberStatsGroup& group, SNNumberVariants& variants)
	{
		SNNumberStats final_res;
		for (uint32_t i = 0; i < group.size(); ++i)
		{
			if (i == 0)
				final_res = group[0];
			else
				CombineStats(group[i], final_res);
		}

		FormatMatcher.MatchNumbers(final_res, variants);

		OutputDebugStringA("Final result\r\n");

		for (auto& v : variants)
		{
			OutputDebugStringA(v.Number.c_str());
			OutputDebugStringA("\r\n");
		}
	}
	//-----------------------------------------------------------------

	void SNNumberRecognizer::SNStatsCombiner::CombinePredictionResults(const ANNPredictionResults& src, ANNPredictionResults& dst)
	{
		for (auto& d : dst)
		{
			for (auto& s : src)
			{
				if (s.Symbol == d.Symbol)
				{
					if (s.Weight > 0.0)
						d.Weight += s.Weight;
				}
			}
		}

		dst.Sort();
	}
}
//-----------------------------------------------------------------