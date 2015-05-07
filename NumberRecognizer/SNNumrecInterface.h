#ifndef SNNumrecInterface_h__
#define SNNumrecInterface_h__
//--------------------------------------------------------------------------
#include <stdint.h>
//#include <windows.h>
#include "SNNumRecExports.h"
//--------------------------------------------------------------------------
#pragma pack(push, 1)
//--------------------------------------------------------------------------

struct SNNumberRecognizerInputFrame
{
	char* RGB32Image;
	uint32_t Width;
	uint32_t Height;
	uint64_t FrameID;
	float ROIX;
	float ROIY;
	float ROIWidth;
	float ROIHeight;

	SNNumberRecognizerInputFrame()
	{
		ROIX = 0.0f;
		ROIY = 0.0f;
		ROIWidth = 1.0f;
		ROIHeight = 1.0f;
	}
};
//--------------------------------------------------------------------------
#pragma pack(pop)

//class SNNumrecInterface
//{
//public:
//	SNNumrecInterface(char* library_name)
//	{
//		Library = LoadLibraryA(library_name);
//		if (Library != INVALID_HANDLE_VALUE)
//		{
//			//CreateSNAffineVadll = (SNNumrecCreate*)GetProcAddress(VALibrary, "SNAffineVadllCreate");
//			//DeleteSNAffineVadll = (SNAffineVadllDelete*)GetProcAddress(VALibrary, "SNAffineVadllDelete");
//			//SNAffineVadllRGB32Image = (SNAffineTransformationRGB32Image*)GetProcAddress(VALibrary, "SNAffineTransformationRGB32Image");
//		}
//	}
//
//	~SNNumrecInterface()
//	{
//
//	}
//private:
//	HMODULE Library;
//	SNNumrecCreate* CreateProc;
//};
//--------------------------------------------------------------------------


#endif // SNNumrecInterface_h__
