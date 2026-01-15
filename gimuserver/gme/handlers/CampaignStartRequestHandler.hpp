#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class CampaignStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "6Y0gaPQN"; }
	const char* GetAesKey() const override { return "WM6yr4ej"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
