#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRaidBattleProgressRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "v87b3Diq"; }
	const char* GetAesKey() const override { return "Da0m39b1"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
