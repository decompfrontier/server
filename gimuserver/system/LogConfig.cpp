#include "LogConfig.hpp"

void LogConfig::ParseFromJson(const Json::Value& v)
{
	Enable = v["enable"].asBool();
	if (Enable)
	{
		RequestPath = v["request_path"].asCString();
		ResponsePath = v["response_path"].asCString();
		Dlc404File = v["dlc_error_file"].asCString();
	}
}

