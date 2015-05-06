#include "SNPlateRecognizer.h"
#include "opencv2\imgproc\imgproc.hpp"
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

	void SNPlateRecognizer::RecognizePlate(SNPlate& plate, SNNumberVariants& variants)
	{
		Segmentor.Segment(plate, 100, 255, -1);

		SNPlateModels best_models;

		int32_t best_group_to_start = 0;

		if (ModelMatcher.MatchModel3(plate, best_models))
		{
			std::string region/* = RecognizeRegion(image, best_model)*/;

			SNNumberStats stats;
			stats.FrameID = plate.FrameID;
			stats.PlateRect = plate.GlobalRect;
			stats.Plate = plate.PlateImage;

			SNNumberRecognizer::ANNPredictionResults results;

			for (int i = best_group_to_start; i < plate.FigureGroups.size(); ++i)
			{
				stats.push_back(SNSymbolStats());

				for (auto& f : plate.FigureGroups[i])
				{
					cv::Mat symbol = cv::Mat(plate.PlateImage, cv::Rect(f.left(), f.top(), f.Width() + 1, f.Height() + 1)).clone();

					Predictor.Predict(SNNumberRecognizer::DigitsAlphabet, symbol, Eval, results);
					if (stats.back().DigitsStats.empty())
						stats.back().DigitsStats = results;
					else
						StatsCombiner.CombinePredictionResults(results, stats.back().DigitsStats);

					results.clear();

					Predictor.Predict(SNNumberRecognizer::LettersAlphabet, symbol, Eval, results);
					
					if (stats.back().LetterStats.empty())
						stats.back().LetterStats = results;
					else
						StatsCombiner.CombinePredictionResults(results, stats.back().LetterStats);
					
					results.clear();

					char best_char = stats.back().LetterStats.front().Symbol;
					char best_digit = stats.back().DigitsStats.front().Symbol;
					int r = 0;
				}
			}

			/*for (auto m : best_model)
			{
				stats.push_back(SNSymbolStats());

				cv::Mat symbol = cv::Mat(image, cv::Rect(m.x, m.y, m.width + 1, m.height + 1)).clone();

				for (int32_t dx = -3; dx <= 3; ++dx)
				{
					for (int32_t dy = -3; dy <= 3; ++dy)
					{
						for (int32_t dw = -3; dw <= 3; ++dw)
						{
							for (int32_t dh = -3; dh <= 3; ++dh)
							{
								symbol = cv::Mat(image, cv::Rect(m.x + dx, m.y + dy, m.width + dw, m.height + dh)).clone();

								if (symbol.rows)
								{
									Predictor.Predict(SNNumberRecognizer::DigitsAlphabet, symbol, Eval, results);

									if (results.front().Weight > 0.8)
									{
										int r = 0;
									}

									stats.back().DigitsStats = results;
									results.clear();

									Predictor.Predict(SNNumberRecognizer::LettersAlphabet, symbol, Eval, results);

									if (results.front().Weight > 0.8)
									{
										int r = 0;
									}

									stats.back().LetterStats = results;
									results.clear();
								}
							}
						}
					}
				}
			}*/

			StatsCombiner.CombineStats(stats);

			/*FormatMatcher.MatchNumbers(stats, variants);

			for (auto& r : variants)
			{
			r.Number += region;
			}*/
		}
	}
	//----------------------------------------------------------------------------------

	bool SNPlateRecognizer::InitRecognizer(const char* config)
	{
		// return false;
		
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
					cv::Rect symbol_rect = best_plate_model[region_index + i].SymbolRect + cv::Point(x, y);

					if (symbol_rect.y >= 0 && symbol_rect.br().x < gray_image.cols && symbol_rect.x >= 0 && symbol_rect.br().y < gray_image.rows)
					{
						cv::Mat symbol = cv::Mat(gray_image, symbol_rect);

						ANNPredictionResults results;
						Predictor.Predict(SNNumberRecognizer::DigitsAlphabet, symbol, Eval, results);

						stats[i].DigitsStats = results;
						
						results.clear();
					}
				}
			}
		}

		for (int32_t i = 0; i < region_size; ++i)
		{
			float max_weight = 0.0f;
			char best_char = '*';

			if (!stats[i].DigitsStats.empty())
			{
				best_char = stats[i].DigitsStats.front().Symbol;
				max_weight = stats[i].DigitsStats.front().Weight;
			}
			
			res += best_char;
		}

		return res;
	}
	//----------------------------------------------------------------------------------

	void SNPlateRecognizer::CheckResults(const uint64_t& frame_id)
	{
		StatsCombiner.CheckResults(ModelMatcher.Model, frame_id);
	}
	//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------