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
	SNNumberVariant var;
	var.Weight = 0.0f;

	if (stats.size() < Format.size())
		return;

	for (int j = 0; j < Format.size(); ++j)
	{
		float weight = 0.0f;
		char best_char = '*';

		if (Format[j] == SNNumberRecognizer::DigitsAlphabet)
		{
			if (!stats[j].DigitsStats.empty())
			{
				weight = stats[j].DigitsStats.front().Weight;
				if (weight > 0.0)
					best_char = stats[j].DigitsStats.front().Symbol;
			}
		}

		if (Format[j] == SNNumberRecognizer::LettersAlphabet)
		{
			if (!stats[j].LetterStats.empty())
			{
				weight = stats[j].LetterStats.front().Weight;
				if (weight > 0.0)
					best_char = stats[j].LetterStats.front().Symbol;
			}

		}

		var.Number += best_char;
		var.Weight += weight;
	}

	result.push_back(var);

	sort(result.begin(), result.end(),
		[](const SNNumberVariant & a, const SNNumberVariant & b) -> bool
	{
		return a.Weight > b.Weight;
	});
}
//--------------------------------------------------------------