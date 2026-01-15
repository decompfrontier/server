#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRaidMissionBattleEndRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "v83Diq7b"; }
	const char* GetAesKey() const override { return "m39bDa01"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
