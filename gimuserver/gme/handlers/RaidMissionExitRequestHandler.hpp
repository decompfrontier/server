#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidMissionExitRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "a12v0RcA"; }
	const char* GetAesKey() const override { return "14GMJFIN"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
