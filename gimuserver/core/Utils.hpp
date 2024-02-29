#pragma once

#include <drogon/HttpRequest.h>
#include <drogon/Session.h>
#include <gme/GmeTypes.hpp>

namespace Utils
{
	void DumpInfoToDrogon(const drogon::HttpRequestPtr& rq, const std::string& ip);
	std::string GetDrogonBindHostname();
	std::string GetDrogonHttpBindHostname();
	std::string RandomUserID();
	std::string RandomAccountID();
	void AppendJsonReqToFile(const Json::Value& v, const std::string& group);
	void AppendJsonResToFile(const Json::Value& v, const std::string& group);
}
