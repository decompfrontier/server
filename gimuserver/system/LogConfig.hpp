#pragma once

#include <json/value.h>

struct LogConfig
{
	bool Enable;
	std::string RequestPath;
	std::string ResponsePath;
	std::string Dlc404File;

	explicit LogConfig() : Enable(false) {}

	void ParseFromJson(const Json::Value& v);
};
