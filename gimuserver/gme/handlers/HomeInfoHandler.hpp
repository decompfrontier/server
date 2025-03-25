#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class HomeInfoHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "NiYWKdzs"; } // Used as a group identifying key
	const char* GetAesKey() const override { return "f6uOewOD"; } // Used as encryption key
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
