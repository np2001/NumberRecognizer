#include "SNLBPTrainer.h"
//---------------------------------------------------------------------
SNNumberRecognizer::SNLBPTrainer::SNLBPTrainer()
{
}
//---------------------------------------------------------------------

SNNumberRecognizer::SNLBPTrainer::~SNLBPTrainer()
{
}
//---------------------------------------------------------------------

void SNNumberRecognizer::SNLBPTrainer::Train(ANNAlphabets& train_alphabets)
{
	struct Feature
	{
		int32_t FeatureCount;
		double PosFeatureProbability;
		double NegFeatureProbability;
		double DeltaProbability;

		Feature()
		{
			FeatureCount = 0;
			PosFeatureProbability = 0.0;
			NegFeatureProbability = 0.0;
			DeltaProbability = 0.0;
		}
	};

	struct FeatureMap : public std::map<int, Feature>
	{
		int SamplesCount;
		FeatureMap()
		{
			SamplesCount = 0;
		}
	};

	typedef std::map<int, FeatureMap> SymbolClassesMap;
	typedef std::map<int, SymbolClassesMap> AlphabetsMap;

	typedef std::map<int, int> TotalFeatureCountMap;


	AlphabetsMap map;
	TotalFeatureCountMap total_feature_map;

	int32_t total_samples_count = 0;

	//Считаем количество семплов, в которых встречается каждая фича и общее число семплов (для каждого класса символа)

	for (auto& alph : train_alphabets)
	{
		for (auto& sym_class : alph.second.SymbolClasses)
		{
			for (auto& sym : sym_class.second)
			{
				for (uint32_t i = 0; i < sym.Features.cols; ++i)
				{
					uint8_t s = sym.Features.at<float>(0, i);

					int32_t feature_id = i * 256 + s;
					map[alph.first][sym_class.first][feature_id].FeatureCount++;
				}
			}

			map[alph.first][sym_class.first].SamplesCount = sym_class.second.size();

			total_samples_count += sym_class.second.size();
		}
	}

	//Считаем общее количество появлений определенной фичи во всех классах символов
	for (auto& m : map)
	{
		for (auto& s : m.second)
		{
			for (auto& f : s.second)
			{
				TotalFeatureCountMap::iterator tfc = total_feature_map.find(f.first);
				if (tfc == total_feature_map.end())
				{
					total_feature_map[f.first] = f.second.FeatureCount;
				}
				else
				{
					tfc->second += f.second.FeatureCount;
				}
			}
		}
	}

	//Считаем вероятность появления данной фичи в определенном классе (pos), а также вероятность ее появления в других классах (neg)
	for (auto& m : map)
	{
		for (auto& s : m.second)
		{
			for (auto& f : s.second)
			{
				f.second.PosFeatureProbability = f.second.FeatureCount * 1.0 / s.second.SamplesCount;
				f.second.NegFeatureProbability = (total_feature_map[f.first] - f.second.FeatureCount) * 1.0 / (total_samples_count - s.second.SamplesCount);
				f.second.DeltaProbability = f.second.PosFeatureProbability - f.second.NegFeatureProbability;
			}
		}
	}
	
	AlphabetsMap map_copy = map;

	struct LBPModelFeature
	{
		uint32_t FeatureID;
		double DeltaProbability;
	};

	typedef std::list<LBPModelFeature> LBPModel;

	typedef std::map<int, LBPModel> SymbolClassesLBPMap;
	typedef std::map<int, SymbolClassesLBPMap> AlphabetsLBPMap;

	AlphabetsLBPMap lbp_map;

	for (auto& m : map_copy)
	{
		for (auto& s : m.second)
		{
			for (int i = 0; i < 10; ++i)
			{
				double max_delta = 0.0;
				FeatureMap::iterator max_delta_iter;

				for (FeatureMap::iterator f = s.second.begin(); f != s.second.end(); ++f)
				{
					if (abs(f->second.DeltaProbability) > abs(max_delta))
					{
						max_delta = f->second.DeltaProbability;
						max_delta_iter = f;
					};
				}
			
				LBPModelFeature lbp_f;
				lbp_f.DeltaProbability = max_delta;
				lbp_f.FeatureID = max_delta_iter->first;

				lbp_map[m.first][s.first].push_back(lbp_f);
				s.second.erase(max_delta_iter);
			}
		}
	}


	//Распознаем символы
	for (auto& alph : train_alphabets)
	{
		for (auto& sym_class : alph.second.SymbolClasses)
		{
			for (auto& sym : sym_class.second)
			{
				double weight = 0.0;

				for (auto f : lbp_map[alph.first][4])
				{
					uint32_t feature_pos = f.FeatureID / 256;
					uint32_t value = f.FeatureID % 256;

					uint8_t s = sym.Features.at<float>(0, feature_pos);

					if (f.DeltaProbability > 0 && s == value)
					{
						weight += f.DeltaProbability;
					}
					else
					{
						if (f.DeltaProbability < 0 && s != value)
						{
							weight += (-f.DeltaProbability);
						}
					}
				}

				if (alph.first == DigitsAlphabet && sym_class.first == 2)
				{
					int r = 0;
				}
			}
		}
	}
}
//---------------------------------------------------------------------