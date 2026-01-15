#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class TieupCampaignInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "nWBLmp53"; }
	const char* GetAesKey() const override { return "h06IDKYb"; }
	// TODO: Response has 2 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
