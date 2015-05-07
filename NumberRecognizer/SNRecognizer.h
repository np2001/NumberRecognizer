#ifndef SNRecognizer_h__
#define SNRecognizer_h__
//-----------------------------------------------------------------------------------
#include "..\Common\SNNumberRecognizerDefines.h"
#include "SNPlateRecognizer.h"
#include "SNPlateDetector.h"
#include <mutex>
#include "SNRecognizerProcessor.h"
#include "SNNumRecExports.h"
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
		bool Process(SNNumberRecognizerInputFrame& frame);
		bool GetPlateToRecognize(SNRecognizerProcessorQuery& query);
	private:
		void ReleaseFrames(SNNumberRecognizerInputFrame& frame);
	private:
		SNPlateDetector PlateDetector;
		SNRecognizerProcessor* RecProcessor;
		SNImageRetainMap ImageRetainMap;
		SNPlates NotRecognizedPlates;
		std::mutex Mutex;
		uint64_t LastFrameID;
		SNFramesToRelease FramesToRelease;
		SNFinalResults FinalResults;
	};
};

//---------------------------------------------------------------------------------- -
#endif // SNRecognizer_h__

