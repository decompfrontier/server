#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class CampaignSaveRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Utzc3oj5"; }
	const char* GetAesKey() const override { return "6xc3GhQF"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
