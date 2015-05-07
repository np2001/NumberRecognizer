#ifndef SNNumRecExports_h__
#define SNNumRecExports_h__
//---------------------------------------------------------------------------
//--------------------------------------------------------------------------
#pragma pack(push, 1)
//--------------------------------------------------------------------------
struct SNFinalResult
{
	char Number[16];
	uint64_t BestFrameID;
	float Weight;
	float PlateX;
	float PlateY;
	float PlateWidth;
	float PlateHeight;
};
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
	uint64_t FramesToRelease[256];
	int32_t FramesToReleaseCount;
	SNFinalResult Results[16];
	int32_t ResultsCount;
	SNNumberRecognizerInputFrame()
	{
		ROIX = 0.0f;
		ROIY = 0.0f;
		ROIWidth = 1.0f;
		ROIHeight = 1.0f;
		FramesToReleaseCount = sizeof(FramesToRelease) / sizeof(FramesToRelease[0]);
		ResultsCount = sizeof(Results) / sizeof(Results[0]);
	}
};
//--------------------------------------------------------------------------
#pragma pack(pop)

#ifdef SNNUMRECDLL_EXPORTS
#define NUMREC_EXPORTS extern "C" _declspec(dllexport) 
//---------------------------------------------------------------------------
#else
#define NUMREC_EXPORTS typedef   __declspec(dllimport)  
#endif

NUMREC_EXPORTS void* (__stdcall SNNumrecCreate)();
NUMREC_EXPORTS void(__stdcall SNNumrecDelete)   (void* instance);
NUMREC_EXPORTS bool(__stdcall SNNumrecProcess)  (void* instance, SNNumberRecognizerInputFrame* frame);

#ifdef SNNUMRECDLL_EXPORTS

#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif // SNNumRecExports_h__
