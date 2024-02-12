#pragma once

#include <drogon/HttpRequest.h>

namespace Utils
{
	void DumpInfoToDrogon(const drogon::HttpRequestPtr& rq, const std::string& ip);
	std::string GetDrogonBindHostname();
	std::string GetDrogonHttpBindHostname();
	std::string RandomUserID();
	std::string RandomAccountID();
}
