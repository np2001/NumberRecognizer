#ifndef SNRecognizerProcessor_h__
#define SNRecognizerProcessor_h__
//-------------------------------------------------------------------
#include <thread>
#include <mutex>
#include <windows.h>
#include "SNPlateRecognizer.h"
//-------------------------------------------------------------------
namespace SNNumberRecognizer
{
	class SNRecognizerProcessor
	{
	public:
		SNRecognizerProcessor(void* owner);
		~SNRecognizerProcessor();
	private:
		friend void ThreadFunction(void* recognizer);
		void Process();
		SNPlateRecognizer PlateRecognizer;

	private:
		void* Owner;
		bool Working;
		std::mutex Mutex;
		std::thread* Thread;
		SNFramesToRelease FramesToRelease;
		SNFinalResults FinalResults;
	};
}
//-------------------------------------------------------------------
#endif // SNRecognizerProcessor_h__
