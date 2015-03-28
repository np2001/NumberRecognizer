#ifndef SNANNTrainer_h__
#define SNANNTrainer_h__
//------------------------------------------------------------------------------
#include "../Common/SNNumberRecognizerDefines.h"
//------------------------------------------------------------------------------

namespace SNNumberRecognizer
{
	class ANNTrainer
	{
	public:
		ANNTrainer();
		~ANNTrainer();
		void Train(ANNAlphabets& data, int num_layers, SNANNConfigMap& ann_config);
	private:
		CvANN_MLP  ANN;
	};
}
//------------------------------------------------------------------------------
#endif // SNANNTrainer_h__
