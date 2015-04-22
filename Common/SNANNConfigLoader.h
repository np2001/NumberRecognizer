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
		std::string Save(const SNANNConfigMap& config);
		bool Load(const char* data, SNANNConfigMap& config);
	private:
		std::string ToHex(std::string str);
		std::string FromHex(std::string str);
	};
};
//-------------------------------------------------------------
#endif // SNANNConfigLoader_h__
