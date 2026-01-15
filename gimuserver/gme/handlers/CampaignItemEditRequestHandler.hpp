#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class CampaignItemEditRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "W2VU91I7"; }
	const char* GetAesKey() const override { return "2Jd10iwn"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
