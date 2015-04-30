#ifndef SNFormatMatcher_h__
#define SNFormatMatcher_h__
#include <vector>
#include "..\Common\SNNumberRecognizerDefines.h"
#include "SNMasterRecognizerStructs.h"

//--------------------------------------------------------------------------

class SNFormatMatcher
{
public:
	SNFormatMatcher();
	~SNFormatMatcher();
	void MatchNumbers(const SNNumberStats& stats, SNNumberVariants& result);
private:
	SNNumberFormat Format;
};
//--------------------------------------------------------------------------

#endif // SNFormatMatcher_h__
