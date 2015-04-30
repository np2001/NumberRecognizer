#include "SNPlateRecognizer.h"
//----------------------------------------------------------------------------------
namespace SNNumberRecognizer
{
	SNPlateRecognizer::SNPlateRecognizer()
	{
	}
	//----------------------------------------------------------------------------------

	SNPlateRecognizer::~SNPlateRecognizer()
	{
	}
	//----------------------------------------------------------------------------------

	void SNPlateRecognizer::RecognizePlate(const cv::Mat& image, SNNumberVariants& variants)
	{
		SNFigureGroups fgs;
		Segmentor.Segment(image, fgs, 100, 255, -1);

		//cv::Mat debug_image;
		//Segmentor.DebugFigureGroups(image, fgs, debug_image, 2);

		SNPlateModel best_model;
		ModelMatcher.MatchModel(image, fgs, best_model);

		std::string region = RecognizeRegion(image, best_model);

		SNNumberStats stats;
		
		SNNumberRecognizer::ANNPredictionResults results;

		for (auto& fg : fgs)
		{
			stats.push_back(SNSymbolStats());

			for (auto& f : fg)
			{
				cv::Mat symbol = cv::Mat(image, cv::Rect(f.left(), f.top(), f.Width() + 1, f.Height() + 1)).clone();

				Predictor.Predict(SNNumberRecognizer::DigitsAlphabet, symbol, Eval, results);

				//stats.back().DigitsStats[results.front().Symbol] += results.front().Weight;
				stats.back().DigitsStats[results.front().Symbol] += f.GetModelMatchRatio();
				results.clear();

				Predictor.Predict(SNNumberRecognizer::LettersAlphabet, symbol, Eval, results);
				//stats.back().LetterStats[results.front().Symbol] += results.front().Weight;
				stats.back().LetterStats[results.front().Symbol] += f.GetModelMatchRatio();
				results.clear();
			}
		}

		FormatMatcher.MatchNumbers(stats, variants);

		for (auto& r : variants)
		{
			r.Number += region;
		}
	}
	//----------------------------------------------------------------------------------

	bool SNPlateRecognizer::InitRecognizer(const char* config)
	{
		SNANNConfigMap test_config;
		bool res = ConfigLoader.Load(config, test_config);
		if (res)
		{
			Predictor.Load(test_config);
		}
		
		return res;
	}
	//----------------------------------------------------------------------------------

	std::string SNPlateRecognizer::RecognizeRegion(const cv::Mat& gray_image, const SNPlateModel& best_plate_model)
	{
		int32_t region_index = 6;
		int32_t region_size = 2;

		std::string res;

		SNNumberStats stats;

		for (int32_t i = 0; i < region_size; ++i)
		{
			stats.push_back(SNSymbolStats());
		}

		for (int32_t y = -3; y <= 3; ++y)
		{
			for (int32_t x = -3; x <= 3; ++x)
			{
				for (int32_t i = 0; i < region_size; ++i)
				{
					cv::Mat symbol = cv::Mat(gray_image, best_plate_model[region_index + i] + cv::Point(x, y));
					ANNPredictionResults results;
					Predictor.Predict(SNNumberRecognizer::DigitsAlphabet, symbol, Eval, results);

					stats[i].DigitsStats[results.front().Symbol] += 1.0f;
					results.clear();
				}
			}
		}

		for (int32_t i = 0; i < region_size; ++i)
		{
			float max_weight = 0.0f;
			char best_char;
			for (auto j : stats[i].DigitsStats)
			{
				if (j.second > max_weight)
				{
					max_weight = j.second;
					best_char = j.first;
				}
			}

			res += best_char;
		}

		return res;
	}
	//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------