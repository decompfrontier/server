#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class VortexArenaClaimDailyRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "j3cyKi8o"; }
	const char* GetAesKey() const override { return "abI7o6dT"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
