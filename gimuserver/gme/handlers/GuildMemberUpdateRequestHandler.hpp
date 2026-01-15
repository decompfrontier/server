#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildMemberUpdateRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "ad81b8at"; }
	const char* GetAesKey() const override { return "2b1bDo2m"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
