#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRaidBattleOutpostRelocateRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "bka03Bi1"; }
	const char* GetAesKey() const override { return "F91Dalb8"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
