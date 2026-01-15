#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class DailyTaskClaimRewardRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "oP3bn47e"; }
	const char* GetAesKey() const override { return "ut0j9h3K"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
