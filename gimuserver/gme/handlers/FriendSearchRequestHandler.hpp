#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FriendSearchRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "umNt8M7i"; }
	const char* GetAesKey() const override { return "qa3sHuDgKxV5nI1F"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
