#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class CampaignBattleEndRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "pTNB6yw3"; }
	const char* GetAesKey() const override { return "t06HFsXP"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
