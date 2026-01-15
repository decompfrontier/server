#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRaidMemberDeployRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Z3d9b0ew"; }
	const char* GetAesKey() const override { return "Bd83Dakb"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
