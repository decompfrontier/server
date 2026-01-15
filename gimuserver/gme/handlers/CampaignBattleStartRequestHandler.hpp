#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class CampaignBattleStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "h1RjcD3S"; }
	const char* GetAesKey() const override { return "4CKoVAq0"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
