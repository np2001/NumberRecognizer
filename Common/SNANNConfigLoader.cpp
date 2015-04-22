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

	std::string SNANNConfigLoader::ToHex(std::string str)
	{
		std::string res;
		for (int i = 0; i < str.size(); ++i)
		{
			char c[20];
			sprintf_s(c, sizeof(c), "%2.2X", str[i]);

			res += c;
		}

		return res;
	}
	//----------------------------------------------------------------------------

	std::string SNANNConfigLoader::FromHex(std::string str)
	{
		std::string res;
		for (int i = 0; i < str.size(); i += 2)
		{
			char src[20];
			int32_t dst;
			memcpy(src, str.c_str() + i, 2);
			src[2] = 0;

			std::stringstream ss;
			ss << std::hex << src;
			ss >> dst;

			res += dst;
		}

		return res;
	}
	//----------------------------------------------------------------------------

	std::string SNANNConfigLoader::Save(const SNANNConfigMap& config)
	{
		std::string res = "<ann_config>\r\n";

		char str[100];

		for (auto c : config)
		{
			sprintf_s(str, sizeof(str), "<alph id=\"%i\" data=\"", (int32_t)c.first);
			
			res += str + ToHex(c.second.ConfigString) + "\" />\r\n";

			int r = 0;
		}

		return res;

		res += "</ann_config>\r\n";
	}
	//----------------------------------------------------------------------------

	bool SNANNConfigLoader::Load(const char* data, SNANNConfigMap& config)
	{
		TiXmlDocument doc;
		doc.Parse(data);

		for (TiXmlElement* n = doc.RootElement()->FirstChildElement("alph"); n != NULL; n = n->NextSiblingElement("alph"))
		{
			const char* text = n->Attribute("data");
			const char* id = n->Attribute("id");

			std::string res = FromHex(text);
			config[(AlphabetTypes)atoi(id)].ConfigString = res;
		}

		return true;
	}
	//----------------------------------------------------------------------------
}
