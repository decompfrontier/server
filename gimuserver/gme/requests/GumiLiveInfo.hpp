#pragma once

#include <string>

#include <json/value.h>

// BaseRequest::addGumiliveParams
REQUEST_NS_BEGIN
struct GumiLiveInfo
{
	std::string gumiLiveUserID;
	std::string gumiLiveToken;
	std::string facebookID;
    std::string userID;

	void Deserialize(const Json::Value& root)
	{
		gumiLiveUserID = root.get("iN7buP2h", "").asCString();
		gumiLiveToken = root.get("iN7buP1i", "").asCString();
		facebookID = root.get("K29dp2Q", "").asCString();
		userID = root.get("uJP4aeg9", "").asCString();
	}
};
REQUEST_NS_END
