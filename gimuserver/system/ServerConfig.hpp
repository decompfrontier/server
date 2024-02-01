#pragma once

#include <json/value.h>

struct ServerConfig
{
	std::string Wallpaper;
	int GameVersion;

	void ParseFromJson(const Json::Value& v);
};
