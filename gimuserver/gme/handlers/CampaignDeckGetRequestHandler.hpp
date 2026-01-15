#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class CampaignDeckGetRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "C3a0VnQK"; }
	const char* GetAesKey() const override { return "q2ZtYJ6P"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
