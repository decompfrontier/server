#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FriendGetAgreeRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "aVqJrR08"; }
	const char* GetAesKey() const override { return "H4zJCK2jPeaskVdf"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
