#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRaidRoomPartSkillListRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "26ZGiseY"; }
	const char* GetAesKey() const override { return "00dU9t9M"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
