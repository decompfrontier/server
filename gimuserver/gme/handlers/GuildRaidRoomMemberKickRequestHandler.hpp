#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRaidRoomMemberKickRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "M2dD4b0A"; }
	const char* GetAesKey() const override { return "I2ixn4Ac"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
