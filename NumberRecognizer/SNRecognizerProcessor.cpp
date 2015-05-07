#include "SNRecognizerProcessor.h"
#include <windows.h>
#include <iostream>
#include "..\Common\ann_config.h"
#include "..\Common\SNStringEncryptor.h"
#include "SNRecognizer.h"
//-----------------------------------------------------------------
namespace SNNumberRecognizer
{
	SNRecognizerProcessor::SNRecognizerProcessor(void* owner)
	{
		SNStringEncryptor se;
		se.Crypt(ann_config);

		PlateRecognizer.InitRecognizer(ann_config);

		Mutex.lock();
		Owner = owner;
		Working = true;
		Mutex.unlock();
		Thread = new std::thread(ThreadFunction, this);
	}
	//-----------------------------------------------------------------

	SNRecognizerProcessor::~SNRecognizerProcessor()
	{
		Mutex.lock();
		Working = false;
		Mutex.unlock();

		Thread->join();

		delete Thread;
	}
	//-----------------------------------------------------------------

	void SNRecognizerProcessor::Process()
	{
		while (true)
		{
			bool working = false;
			Mutex.lock();
			working = Working;
			Mutex.unlock();

			if (!Working)
				break;

			SNRecognizer* rec = (SNRecognizer*)Owner;

			SNRecognizerProcessorQuery query;
			query.FramesToRelease = FramesToRelease;
			FramesToRelease.clear();
			query.FinalResults = FinalResults;
			FinalResults.clear();

			if (!rec->GetPlateToRecognize(query))
			{
				Sleep(1000);
			}
			else
			{
				SNNumberVariants vars;
				PlateRecognizer.RecognizePlate(query.PlateToRecognize, vars, FramesToRelease);
			}

			PlateRecognizer.CheckResults(query.LastFrameID, FramesToRelease, FinalResults);

		//	OutputDebugStringA("Thread\r\n");
			//std::cout << "!!!\r\n";
			//Sleep(1000);
		}
	}
	//-----------------------------------------------------------------

	void ThreadFunction(void* processor)
	{
		SNRecognizerProcessor* pr = (SNRecognizerProcessor*)processor;
		pr->Process();
	}
}
//-----------------------------------------------------------------