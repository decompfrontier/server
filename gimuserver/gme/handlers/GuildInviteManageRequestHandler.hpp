#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildInviteManageRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "1D8bba8D"; }
	const char* GetAesKey() const override { return "bUd2bd0e"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
