#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class DailyRewardClaimRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "AT_DAILY_REWARD"; }
	const char* GetAesKey() const override { return "ATK_DAILY_REWARD"; }
	// TODO: Response has 1 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
