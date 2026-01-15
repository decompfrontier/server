#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidCampRestEndRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "f9Dh4djx"; }
	const char* GetAesKey() const override { return "0TCX6S4R"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
