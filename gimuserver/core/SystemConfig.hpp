#pragma once

#include <string>

struct SystemConfig
{
	struct Log
	{
		bool enable;
		std::string request_path;
		std::string response_path;
		std::string dlc_error_file;
	};

	struct Server
	{
		std::string wallpaper_banner;
		unsigned int game_version;
		std::string notice_url;
	};

	struct System
	{
		std::string content_root;
		std::string gme_sqlite_path;
		unsigned int session_timeout;
		std::string mst_root;
	};

	System system;
	Server server;
	Log log;
};
