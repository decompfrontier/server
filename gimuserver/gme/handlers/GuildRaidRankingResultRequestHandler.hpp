#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRaidRankingResultRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "W1Daxfnz"; }
	const char* GetAesKey() const override { return "eMMfFDVJ"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
