#ifndef SNANNConfigLoader_h__
#define SNANNConfigLoader_h__
//-------------------------------------------------------------
#include "SNNumberRecognizerDefines.h"
//-------------------------------------------------------------
namespace SNNumberRecognizer
{
	class SNANNConfigLoader
	{
	public:
		SNANNConfigLoader();
		~SNANNConfigLoader();
		void Save(const SNANNConfigMap& config);
	};
};
//-------------------------------------------------------------
#endif // SNANNConfigLoader_h__
