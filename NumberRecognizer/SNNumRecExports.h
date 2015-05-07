#ifndef SNNumRecExports_h__
#define SNNumRecExports_h__
//---------------------------------------------------------------------------
#ifdef SNNUMRECDLL_EXPORTS
#define NUMREC_EXPORTS extern "C" _declspec(dllexport) 
//---------------------------------------------------------------------------
//#include "SNRecognizer.h"
//---------------------------------------------------------------------------
#else
#define NUMREC_EXPORTS typedef   __declspec(dllimport)  
#endif

NUMREC_EXPORTS void* (__stdcall SNNumrecCreate)();
NUMREC_EXPORTS void(__stdcall SNNumrecDelete)   (void* instance);
NUMREC_EXPORTS bool(__stdcall SNNumrecProcess)  (void* instance, char* buffer, int out_width, int out_height);

#ifdef SNNUMRECDLL_EXPORTS

#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif // SNNumRecExports_h__
