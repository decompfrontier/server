#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class CampaignDeckEditRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "D74TYRf1"; }
	const char* GetAesKey() const override { return "e2k4s6jc"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
