#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildJoinedListRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "3890ab5j"; }
	const char* GetAesKey() const override { return "820b38z5"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
