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
		for (size_t i = 0; i < str.size(); ++i)
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
		res.reserve(str.size());

		for (size_t i = 0; i < str.size(); i += 2)
		{
			uint8_t d = 0;
			for (int j = 0; j < 2; ++j)
			{
				switch (str[i + j])
				{
				case '0': d = ((d << 4) | 0x00); break;
				case '1': d = ((d << 4) | 0x01); break;
				case '2': d = ((d << 4) | 0x02); break;
				case '3': d = ((d << 4) | 0x03); break;
				case '4': d = ((d << 4) | 0x04); break;
				case '5': d = ((d << 4) | 0x05); break;
				case '6': d = ((d << 4) | 0x06); break;
				case '7': d = ((d << 4) | 0x07); break;
				case '8': d = ((d << 4) | 0x08); break;
				case '9': d = ((d << 4) | 0x09); break;
				case 'A': d = ((d << 4) | 0x0A); break;
				case 'B': d = ((d << 4) | 0x0B); break;
				case 'C': d = ((d << 4) | 0x0C); break;
				case 'D': d = ((d << 4) | 0x0D); break;
				case 'E': d = ((d << 4) | 0x0E); break;
				case 'F': d = ((d << 4) | 0x0F); break;
				default:
					break;
				}
			}
			
			res += d;
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
			
			res += str + ToHex(c.second.ConfigString) + "\">\r\n";

			for (auto cc : c.second.CharClassIDs)
			{
				if (cc.second.DisplaySymbol == 0)
					cc.second.DisplaySymbol = '*';

				sprintf_s(str, sizeof(str), "<symclass id=\"%i\" displaysymbol=\"%c\"/>\r\n", (int32_t)cc.first, cc.second.DisplaySymbol);
				res += str;
			}

			res += "</alph>\r\n";
			int r = 0;
		}

		res += "</ann_config>\r\n";

		return res;
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

			for (TiXmlElement* sym_class = n->FirstChildElement("symclass"); sym_class != NULL; sym_class = sym_class->NextSiblingElement("symclass"))
			{
				const char* sym = sym_class->Attribute("displaysymbol");
				const char* sym_class_id = sym_class->Attribute("id");
				config[(AlphabetTypes)atoi(id)].CharClassIDs[atoi(sym_class_id)].DisplaySymbol = sym[0];
			}
		}

		return true;
	}
	//----------------------------------------------------------------------------
}
