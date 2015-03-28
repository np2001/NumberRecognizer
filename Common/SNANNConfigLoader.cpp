#include "SNANNConfigLoader.h"
#include "tinyxml.h"

namespace SNNumberRecognizer
{
	SNANNConfigLoader::SNANNConfigLoader()
	{

	}
	//----------------------------------------------------------------------------

	SNANNConfigLoader::~SNANNConfigLoader()
	{

	}
	//----------------------------------------------------------------------------

	void SNANNConfigLoader::Save(const SNANNConfigMap& config)
	{
		std::string res = "<ann_config>\r\n";

		for (auto c : config)
		{
			res += c.second.ConfigString.c_str();
		}

		res += "</ann_config>\r\n";

		TiXmlDocument doc;
		doc.Parse(res.c_str());

		for (TiXmlElement* n = doc.RootElement()->FirstChildElement("opencv_storage"); n != NULL; n = n->NextSiblingElement("opencv_storage"))
		{
			const char* text = n->GetText();
			int r = 0;
		}

		int r = 0;
	}
	//----------------------------------------------------------------------------
}
