#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRaidInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Ie01B83k"; }
	const char* GetAesKey() const override { return "P93Db8q1"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
