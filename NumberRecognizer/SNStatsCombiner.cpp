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

	void SNStatsCombiner::CombineStats(const SNPlate& plate, SNFramesToRelease& frames_to_release)
	{
		bool matched_group_found = false;
		for (auto& nggs : NumberStatsGroups)
		{
			if (IsGroupMatched(nggs, plate))
			{
				nggs.push_back(plate);
				matched_group_found = true;
				nggs.LastFrameID = plate.FrameID;
				nggs.LastRect = plate.GlobalRect;
				nggs.Plate = plate.PlateImage;

				if (plate.Stats.TotalWeight > nggs.BestWeight)
				{
					nggs.BestWeight = plate.Stats.TotalWeight;
					frames_to_release.push_back(nggs.BestFrameID);
					nggs.BestFrameID = plate.FrameID;
				}
				else
				{
					frames_to_release.push_back(plate.FrameID);
				}
				break;
			}
		}
		
		if (!matched_group_found)
		{
			SNNumberStatsGroup new_group;
			new_group.BestFrameID = plate.FrameID;
			new_group.BestWeight = plate.Stats.TotalWeight;
			new_group.LastFrameID = plate.FrameID;
			new_group.BestPlateRect = plate.GlobalRect;
			new_group.LastRect = plate.GlobalRect;
			new_group.Plate = plate.PlateImage;
			
			new_group.push_back(plate);
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
	}
	//-----------------------------------------------------------------

	bool SNStatsCombiner::IsGroupMatched(const SNNumberStatsGroup& group, const SNPlate& plate)
	{
		bool res = false;
		float dx = abs(group.LastRect.x - plate.GlobalRect.x);
		if (dx < 120)
		{
			res = true;
		}

		if (group.size() > 1)
		{
			int sign_y1 = (group.LastRect.y - plate.GlobalRect.y) ? 1 : -1;
			int sign_y2 = (group.front().GlobalRect.y - group.back().GlobalRect.y) ? 1 : -1;
			if (sign_y1 != sign_y2)
			{
				res = false;
			}
		}

		return res;
	}
	//-----------------------------------------------------------------

	void SNStatsCombiner::CheckResults(const SNPlateModel& model, const uint64_t frame_id, SNFramesToRelease& frames_to_release, SNFinalResults& results)
	{
		for (auto g = NumberStatsGroups.begin(); g != NumberStatsGroups.end();)
		{
			if (frame_id - g->LastFrameID > 5000000)
			{
				OutputIntermediateResults(model, *g);
				
				SNNumberVariants variants;
				DetectFinalResult(model, *g, variants);
				
				for (auto& v : variants)
				{
					SNFinalResult res;
					res.BestFrameID = g->BestFrameID;
					res.BestPlateRect = g->BestPlateRect;
					res.BestPlate = g->Plate;
					res.Number = v.Number;
					res.Weight = v.Weight;
					results.push_back(res);
				}

				frames_to_release.push_back(g->BestFrameID);
				g = NumberStatsGroups.erase(g);
			}
			else
			{
				g++;
			}
		}
	}
	//-----------------------------------------------------------------

	void SNStatsCombiner::OutputIntermediateResults(const SNPlateModel& model, const SNNumberStatsGroup& group)
	{
		OutputDebugStringA("Intermediate results\r\n");

		for (auto& num : group)
		{
			SNNumberVariants variants;
			FormatMatcher.MatchNumbers(model, num.Stats, variants);

			for (auto& v : variants)
			{
				OutputDebugStringA(v.Number.c_str());
				OutputDebugStringA("\r\n");
			}
		}
	}
	//-----------------------------------------------------------------

	void SNStatsCombiner::DetectFinalResult(const SNPlateModel& model, const SNNumberStatsGroup& group, SNNumberVariants& variants)
	{
		SNPlate final_res;

		for (int i = 0; i < model.size(); ++i)
			final_res.Stats.push_back(SNSymbolStats());

		for (uint32_t i = 0; i < group.size(); ++i)
		{
			CombineStats(group[i].Stats, final_res.Stats);
		}

		if (!final_res.Stats.empty())
		{
			FormatMatcher.MatchNumbers(model, final_res.Stats, variants);

			/*for (auto& v : variants)
			{
				if (v.Weight > 10)
				{
					OutputDebugStringA("Final result\r\n");

					char c[100];
					sprintf_s(c, 100, "%s %2.2f\r\n", v.Number.c_str(), v.Weight);
					OutputDebugStringA(c);
				}
			}*/
		}
	}
	//-----------------------------------------------------------------

	void SNNumberRecognizer::SNStatsCombiner::CombinePredictionResults(const ANNPredictionResults& src, ANNPredictionResults& dst)
	{
		if (dst.empty())
		{
			dst = src;
		}
		else
		{
			for (auto& s : src)
			{
				bool symbol_found_in_dst = false;

				for (auto& d : dst)
				{
					if (s.Symbol == d.Symbol)
					{
						if (s.Weight > 0.0)
							d.Weight += s.Weight;
						symbol_found_in_dst = true;
						break;
					}
				}

				if (!symbol_found_in_dst)
				{
					ANNPredictionResult res;
					res.Symbol = s.Symbol;
					res.Weight = s.Weight;
					dst.push_back(res);
				}
			}
		}
		
		dst.Sort();
	}
}
//-----------------------------------------------------------------