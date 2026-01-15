#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildQuestClaimRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "dsRW32K"; }
	const char* GetAesKey() const override { return "Afs43Dc4"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
