#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class UserLoginCampaignInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "5fc8bf2c"; }
	const char* GetAesKey() const override { return "4eb7ce1b"; }
	// TODO: Response has 4 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
