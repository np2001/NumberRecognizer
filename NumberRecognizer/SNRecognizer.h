#ifndef SNRecognizer_h__
#define SNRecognizer_h__
//-----------------------------------------------------------------------------------
#include "..\Common\SNNumberRecognizerDefines.h"
#include "SNPlateRecognizer.h"
#include "SNPlateDetector.h"
//-----------------------------------------------------------------------------------

namespace SNNumberRecognizer
{
	class SNRecognizer
	{
	public:
		SNRecognizer();
		~SNRecognizer();
		void Process(const SNNumberRecognizerInputFrame& frame);
	private:
		SNPlateRecognizer PlateRecognizer;
		SNPlateDetector PlateDetector;
	};
};
//---------------------------------------------------------------------------------- -
#endif // SNRecognizer_h__

