#pragma once

#include <drogon/HttpRequest.h>
#include <drogon/Session.h>

namespace Utils
{

	std::string RandomUserID();
	std::string RandomAccountID();
	void AppendJsonReqToFile(std::string_view json, std::string_view group);
	void AppendJsonResToFile(std::string_view json, std::string_view group);
	void AddMissingDlcFile(std::string_view file);
}
