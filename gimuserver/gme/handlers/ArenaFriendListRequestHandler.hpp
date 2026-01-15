#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ArenaFriendListRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "xkd9H50w"; }
	const char* GetAesKey() const override { return "L3TDhrn2"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
