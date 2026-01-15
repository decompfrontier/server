#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ChallengeArenaRankingRewardRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "tct1Jogs"; }
	const char* GetAesKey() const override { return "RRxXMpzm"; }
	// TODO: Response has 58 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
