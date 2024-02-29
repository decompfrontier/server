#include "ServerConfig.hpp"

void ServerConfig::ParseFromJson(const Json::Value& v)
{
	Wallpaper = v["wallpaper_banner"].asString();
	GameVersion = v["game_version"].asInt();
	NoticeUrl = v["notice_url"].asString();
}

