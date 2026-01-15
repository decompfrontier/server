#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildMemberGuildInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "38bSeq81"; }
	const char* GetAesKey() const override { return "h8TmR1bi"; }
	// TODO: Response has 12 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
