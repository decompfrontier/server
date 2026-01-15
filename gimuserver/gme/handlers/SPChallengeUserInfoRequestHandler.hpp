#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class SPChallengeUserInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "G3J9Domt"; }
	const char* GetAesKey() const override { return "6QJwgU3V"; }
	// TODO: Response has 3 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
