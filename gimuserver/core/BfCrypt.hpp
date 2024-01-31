#pragma once

#include <json/value.h>

namespace BfCrypt
{
	std::string CryptSREE(const Json::Value& v);

	std::string CryptGME(const Json::Value& v, const std::string& key);
	Json::Value DecryptGME(const std::string& in, const std::string& key);
}
