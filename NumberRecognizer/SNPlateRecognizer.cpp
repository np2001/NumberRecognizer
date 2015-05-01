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

	void SNPlateRecognizer::RecognizePlate(const cv::Mat& image, SNFigureGroups& fgs, SNNumberVariants& variants)
	{
		SNFigureGroup fg;
		Segmentor.Segment(image, fg, 100, 255, -1);

		Segmentor.GroupByIntersect(fg, fgs);

		SNPlateModel best_model;

		int32_t best_group_to_start;

		
		if (ModelMatcher.MatchModel2(image, fgs, best_group_to_start))
		{
			//for (auto fg: fgs)
			//ModelMatcher.MatchModel(image, fg);

			std::string region/* = RecognizeRegion(image, best_model)*/;

			SNNumberStats stats;

			SNNumberRecognizer::ANNPredictionResults results;

			for (int i = best_group_to_start; i < fgs.size(); ++i)
			{
				stats.push_back(SNSymbolStats());

				for (auto& f : fgs[i])
				{
					cv::Mat symbol = cv::Mat(image, cv::Rect(f.left(), f.top(), f.Width() + 1, f.Height() + 1)).clone();

					if (!symbol.rows)
					{
						int r = 0;
					}

					Predictor.Predict(SNNumberRecognizer::DigitsAlphabet, symbol, Eval, results);

					stats.back().DigitsStats = results;
					//stats.back().DigitsStats[results.front().Symbol] += f.GetModelMatchRatio();
					results.clear();

					Predictor.Predict(SNNumberRecognizer::LettersAlphabet, symbol, Eval, results);
					stats.back().LetterStats = results;
					//stats.back().LetterStats[results.front().Symbol] += f.GetModelMatchRatio();
					results.clear();
				}
			}

			FormatMatcher.MatchNumbers(stats, variants);

			for (auto& r : variants)
			{
				r.Number += region;
			}
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
					cv::Rect symbol_rect = best_plate_model[region_index + i] + cv::Point(x, y);

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

	void SNPlateRecognizer::DebugFigureGroups(const cv::Mat& gray_image, const SNFigureGroups& groups, cv::Mat& out_image, int scale /*= 1*/)
	{
		cvtColor(gray_image, out_image, CV_GRAY2RGB);
		cv:resize(out_image, out_image, out_image.size() * scale);

		for (auto group : groups)
		{
			cv::Scalar color = CV_RGB(rand() % 255, rand() % 255, rand() % 255);
			//// отрисовываем найденные фигуры
			if (!group.empty())
			{
				for (size_t nn = 0; nn < group.size(); ++nn)
				{
					cv::rectangle(out_image, cv::Point(group[nn].left() * scale, group[nn].top() * scale), cv::Point(group[nn].right() * scale, group[nn].bottom() * scale), color, 1);
				}
			}
		}
		//---------------------------------------------------
	}

	//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------