#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ChallengeUserInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "jF3AS4cp"; }
	const char* GetAesKey() const override { return "Nst6MK5m"; }
	// TODO: Response has 8 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
