#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidMissionBattleStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "8UiQj0uF"; }
	const char* GetAesKey() const override { return "1Jhavqb7"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
