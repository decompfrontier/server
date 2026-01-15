#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidFriendRemoveRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "u59zdKCi"; }
	const char* GetAesKey() const override { return "0HJ9ShKp"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
