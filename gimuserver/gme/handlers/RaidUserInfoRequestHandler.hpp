#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidUserInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "jJZ01VQo"; }
	const char* GetAesKey() const override { return "SsQ26dVY"; }
	// TODO: Response has 9 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
