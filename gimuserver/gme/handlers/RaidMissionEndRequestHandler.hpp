#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidMissionEndRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "vgin9Mf1"; }
	const char* GetAesKey() const override { return "95ULkowE"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
