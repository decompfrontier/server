#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRaidRoomBattleStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "7di8aie9"; }
	const char* GetAesKey() const override { return "yh8ak18b"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
