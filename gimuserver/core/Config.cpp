#include "Config.hpp"
#include <json/json.h>
#include <fstream>

bool Config::Load(const std::string& p)
{
	std::ifstream ifs;
	ifs.open(p);

	if (!ifs.is_open())
		return false;

	try
	{
		Json::CharReaderBuilder rb;
		JSONCPP_STRING jss;
		Json::Value root;
		if (!Json::parseFromStream(rb, ifs, &root, &jss))
			return false;

		m_contentRoot = root["content_root"].asString();
		m_wallpaper = root["wallpaper_banner"].asString();
	}
	catch (const std::exception& ex)
	{
		return false;
	}

	return true;
}

Config Config::m_cfg;
