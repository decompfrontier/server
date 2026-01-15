#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRaidRoomUpdatedRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "L3D9eK19"; }
	const char* GetAesKey() const override { return "9Ur3Dkb3"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
