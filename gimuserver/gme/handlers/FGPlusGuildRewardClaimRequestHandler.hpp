#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FGPlusGuildRewardClaimRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Sp7umiaY"; }
	const char* GetAesKey() const override { return "thIUXiu5"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
