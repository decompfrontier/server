#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRaidMissionBattleStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "d735ub8o"; }
	const char* GetAesKey() const override { return "bk7eob01"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
