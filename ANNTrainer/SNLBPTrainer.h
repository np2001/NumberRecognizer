#ifndef SNLBPTrainer_h__
#define SNLBPTrainer_h__
//---------------------------------------------------------------------------
#include "..\Common\SNNumberRecognizerDefines.h"
//---------------------------------------------------------------------------

namespace SNNumberRecognizer
{
	class SNLBPTrainer
	{
	public:
		SNLBPTrainer();
		~SNLBPTrainer();
		void Train(ANNAlphabets& train_alphabets);
	};
}
//---------------------------------------------------------------------------

#endif // SNLBPTrainer_h__
