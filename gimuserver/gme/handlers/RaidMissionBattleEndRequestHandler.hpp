#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidMissionBattleEndRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "5Ko2BjPA"; }
	const char* GetAesKey() const override { return "gi6B0m2p"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
