#ifndef SNFormatMatcher_h__
#define SNFormatMatcher_h__
#include <vector>
#include "..\Common\SNNumberRecognizerDefines.h"
#include "SNModelMatcher.h"
//--------------------------------------------------------------------------
namespace SNNumberRecognizer
{
	class SNFormatMatcher
	{
	public:
		SNFormatMatcher();
		~SNFormatMatcher();
		void MatchNumbers(const SNPlateModel& format, const SNNumberStats& stats, SNNumberVariants& result);
	private:

	};
}
//--------------------------------------------------------------------------

#endif // SNFormatMatcher_h__
