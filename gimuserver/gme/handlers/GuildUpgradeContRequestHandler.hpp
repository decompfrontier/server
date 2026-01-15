#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildUpgradeContRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "38bad198"; }
	const char* GetAesKey() const override { return "d38bHiqj"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
