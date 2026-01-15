#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRaidBattleInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "9b98aKj1"; }
	const char* GetAesKey() const override { return "b8dAl1ic"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
