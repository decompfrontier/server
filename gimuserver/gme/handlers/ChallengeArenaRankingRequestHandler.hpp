#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ChallengeArenaRankingRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "L5LN9ytu"; }
	const char* GetAesKey() const override { return "6gTFuTF9"; }
	// TODO: Response has 9 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
