#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRoundBattleSummaryRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "aXPZmq9h"; }
	const char* GetAesKey() const override { return "J90g7sZK"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
