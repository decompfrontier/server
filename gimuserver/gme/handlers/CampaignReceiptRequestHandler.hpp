#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class CampaignReceiptRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "5Imq3wC0"; }
	const char* GetAesKey() const override { return "4DAgP80B"; }
	// TODO: Response has 1 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
