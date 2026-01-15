#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRaidRoomPartSkillSetRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "ZZ9xaJoi"; }
	const char* GetAesKey() const override { return "sdtfUs9y"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
