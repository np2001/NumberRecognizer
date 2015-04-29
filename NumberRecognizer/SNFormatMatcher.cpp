#include "SNFormatMatcher.h"
//--------------------------------------------------------------

SNFormatMatcher::SNFormatMatcher()
{
	Format.push_back(SNNumberRecognizer::LettersAlphabet);
	Format.push_back(SNNumberRecognizer::DigitsAlphabet);
	Format.push_back(SNNumberRecognizer::DigitsAlphabet);
	Format.push_back(SNNumberRecognizer::DigitsAlphabet);
	Format.push_back(SNNumberRecognizer::LettersAlphabet);
	Format.push_back(SNNumberRecognizer::LettersAlphabet);
}
//--------------------------------------------------------------

SNFormatMatcher::~SNFormatMatcher()
{
}
//--------------------------------------------------------------

void SNFormatMatcher::MatchNumbers(const SNNumberStats& stats, SNNumberVariants& result)
{
	for (int i = 0; i <= stats.size() - Format.size(); ++i)
	{
		SNNumberVariant var;
		for (int j = 0; j < Format.size(); ++j)
		{
			float weight = -10000;
			char best_char = '*';

			if (Format[j] == SNNumberRecognizer::DigitsAlphabet)
			{
				for (auto k : stats[i + j].DigitsStats)
				{
					if (k.second > weight)
					{
						best_char = k.first;
						weight = k.second;
					}
				}
			}

			if (Format[j] == SNNumberRecognizer::LettersAlphabet)
			{
				for (auto k : stats[i + j].LetterStats)
				{
					if (k.second > weight)
					{
						best_char = k.first;
						weight = k.second;
					}
				}
			}

			var.Number += best_char;
			var.Weight += weight;
		}

		result.push_back(var);
	}
}
//--------------------------------------------------------------