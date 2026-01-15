#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class CampaignMissionGetRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "RSm6p2d4"; }
	const char* GetAesKey() const override { return "5jzXN7AH"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
