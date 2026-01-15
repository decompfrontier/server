#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ChallengeArenaBattleStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "rbOMSnba"; }
	const char* GetAesKey() const override { return "iVvpULXG"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
