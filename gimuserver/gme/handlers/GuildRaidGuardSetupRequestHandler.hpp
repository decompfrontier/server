#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRaidGuardSetupRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Xdi3ebD9"; }
	const char* GetAesKey() const override { return "TrDi19Bd"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
