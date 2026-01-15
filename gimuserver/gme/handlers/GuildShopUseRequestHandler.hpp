#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildShopUseRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "cXi7b58e"; }
	const char* GetAesKey() const override { return "jK18btd0"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
