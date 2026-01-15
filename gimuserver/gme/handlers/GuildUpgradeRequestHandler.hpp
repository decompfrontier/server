#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildUpgradeRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "a38B82bG"; }
	const char* GetAesKey() const override { return "7Ykwq038"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
