#ifndef SNRecognizer_h__
#define SNRecognizer_h__
//-----------------------------------------------------------------------------------
#include "..\Common\SNNumberRecognizerDefines.h"
#include "SNPlateRecognizer.h"
#include "SNPlateDetector.h"
//-----------------------------------------------------------------------------------

namespace SNNumberRecognizer
{
	//-----------------------------------------------------------------------------------
	typedef std::map<uint64_t, int32_t> SNImageRetainMap;
	//-----------------------------------------------------------------------------------

	class SNRecognizer
	{
	public:
		SNRecognizer();
		~SNRecognizer();

		//¬озвращаем true, если кадр нужно придержать в очереди
		bool Process(const SNNumberRecognizerInputFrame& frame);
	private:
		SNPlateRecognizer PlateRecognizer;
		SNPlateDetector PlateDetector;
		SNImageRetainMap ImageRetainMap;
	};
};
//---------------------------------------------------------------------------------- -
#endif // SNRecognizer_h__

