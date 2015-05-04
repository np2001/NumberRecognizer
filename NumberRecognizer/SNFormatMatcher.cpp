#include "SNFormatMatcher.h"
//--------------------------------------------------------------
namespace SNNumberRecognizer
{
	SNFormatMatcher::SNFormatMatcher()
	{
	}
	//--------------------------------------------------------------

	SNFormatMatcher::~SNFormatMatcher()
	{
	}
	//--------------------------------------------------------------

	void SNFormatMatcher::MatchNumbers(const SNPlateModel& format, const SNNumberStats& stats, SNNumberVariants& result)
	{
		SNNumberVariant var;
		var.Weight = 0.0f;

		if (stats.size() < format.size())
			return;

		for (int j = 0; j < format.size(); ++j)
		{
			float weight = 0.0f;
			char best_char = '*';

			if (format[j].SymbolType == SNNumberRecognizer::DigitsAlphabet)
			{
				if (!stats[j].DigitsStats.empty())
				{
					weight = stats[j].DigitsStats.front().Weight;
					if (weight > 0.0)
						best_char = stats[j].DigitsStats.front().Symbol;
				}
			}

			if (format[j].SymbolType == SNNumberRecognizer::LettersAlphabet)
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
}
//--------------------------------------------------------------