#pragma once

#include <json/value.h>

struct LogConfig
{
	bool Enable;
	std::string RequestPath;
	std::string ResponsePath;

	explicit LogConfig() : Enable(false) {}

	void ParseFromJson(const Json::Value& v);
};
