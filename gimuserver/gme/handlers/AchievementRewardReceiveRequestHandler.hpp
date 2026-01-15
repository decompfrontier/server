#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class AchievementRewardReceiveRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "uq69mTtR"; }
	const char* GetAesKey() const override { return "cbE74zBZ"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
