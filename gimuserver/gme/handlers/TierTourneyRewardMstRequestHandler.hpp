#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class TierTourneyRewardMstRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "gWi2z398"; }
	const char* GetAesKey() const override { return "M2kajT23Yqwp83CX"; }
	// TODO: Response has 7 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
