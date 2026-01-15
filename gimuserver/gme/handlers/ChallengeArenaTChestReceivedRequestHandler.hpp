#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ChallengeArenaTChestReceivedRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "xmzmjcxQ"; }
	const char* GetAesKey() const override { return "L4W9YSex"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
