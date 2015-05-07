#ifndef SNNumrecInterface_h__
#define SNNumrecInterface_h__
//--------------------------------------------------------------------------
#include <stdint.h>
#include <windows.h>
#include "SNNumRecExports.h"

class SNNumrecInterface
{
public:
	SNNumrecInterface(char* library_name)
	{
		Library = LoadLibraryA(library_name);
		
		CreateProc = nullptr;
		DeleteProc = nullptr;

		RecognizerInstance = nullptr;

		if (Library != INVALID_HANDLE_VALUE)
		{
			CreateProc = (SNNumrecCreate*)GetProcAddress(Library, "SNNumrecCreate");
			DeleteProc = (SNNumrecDelete*)GetProcAddress(Library, "SNNumrecDelete");
			ProcessProc = (SNNumrecProcess*)GetProcAddress(Library, "SNNumrecProcess");
			if (CreateProc)
				RecognizerInstance = CreateProc();
	
		}
	}

	bool Process(SNNumberRecognizerInputFrame* frame)
	{
		if (RecognizerInstance)
			return ProcessProc(RecognizerInstance, frame);
		else
			return false;
	}

	~SNNumrecInterface()
	{
		if (RecognizerInstance)
			DeleteProc(RecognizerInstance);
	}

private:
	HMODULE Library;
	void* RecognizerInstance;
	SNNumrecCreate* CreateProc;
	SNNumrecDelete* DeleteProc;
	SNNumrecProcess* ProcessProc;
};
//--------------------------------------------------------------------------


#endif // SNNumrecInterface_h__
