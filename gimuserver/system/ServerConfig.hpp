#pragma once

#include <json/value.h>

struct ServerConfig
{
	explicit ServerConfig() : GameVersion(0) {}

	std::string Wallpaper;
	int GameVersion;
	std::string NoticeUrl;

	void ParseFromJson(const Json::Value& v);
};
