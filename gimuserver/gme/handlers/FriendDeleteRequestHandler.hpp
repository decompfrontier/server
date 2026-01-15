#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FriendDeleteRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "SfMN9w4p"; }
	const char* GetAesKey() const override { return "yWXI80UKQNBZFozw"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
