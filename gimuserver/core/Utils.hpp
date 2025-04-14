#pragma once

#include <drogon/HttpRequest.h>
#include <drogon/Session.h>

namespace Utils
{
	void DumpInfoToDrogon(const drogon::HttpRequestPtr& rq, std::string_view ip);

	inline std::string GetDrogonBindHostname() {
		return drogon::app().getListeners()[0].toIpPort(); // doesn't sound 100% ok
	}

	inline std::string GetDrogonHttpBindHostname() {
		return "http://" + GetDrogonBindHostname() + "/";
	}

	std::string RandomUserID();
	std::string RandomAccountID();
	void AppendJsonReqToFile(std::string_view json, std::string_view group);
	void AppendJsonResToFile(std::string_view json, std::string_view group);
	void AddMissingDlcFile(std::string_view file);
}
