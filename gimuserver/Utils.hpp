#pragma once

#include <drogon/HttpRequest.h>

namespace Utils
{
	void DumpInfoToDrogon(const drogon::HttpRequestPtr& rq, const std::string& ip);
}
