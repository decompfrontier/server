#include "ServerConfig.hpp"

void ServerConfig::ParseFromJson(const Json::Value& v)
{
	Wallpaper = v["wallpaper_banner"].asCString();
	GameVersion = v["game_version"].asInt();
	NoticeUrl = v["notice_url"].asString();
}

