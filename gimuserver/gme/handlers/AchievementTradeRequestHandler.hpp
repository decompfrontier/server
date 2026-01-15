#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class AchievementTradeRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "m9LiF6P2"; }
	const char* GetAesKey() const override { return "0IWC9LVq"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
