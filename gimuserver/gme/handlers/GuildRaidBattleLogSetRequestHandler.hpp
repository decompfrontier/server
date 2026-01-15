#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRaidBattleLogSetRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "36jhZ9YZ"; }
	const char* GetAesKey() const override { return "Zd3d2zGx"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
