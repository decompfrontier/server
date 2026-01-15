#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class TierTourneyClaimRewardRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Ue7kmR38"; }
	const char* GetAesKey() const override { return "23r27AbNW239alHi"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
