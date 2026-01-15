#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRaidBattleLogListRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "MTzXyuFL"; }
	const char* GetAesKey() const override { return "MkV5xHDL"; }
	// TODO: Response has 16 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
