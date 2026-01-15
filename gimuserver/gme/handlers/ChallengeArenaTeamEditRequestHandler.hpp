#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ChallengeArenaTeamEditRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "sXXX6Ec7"; }
	const char* GetAesKey() const override { return "DcsPPuTG"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
