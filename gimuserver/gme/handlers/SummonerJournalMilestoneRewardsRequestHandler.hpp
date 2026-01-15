#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class SummonerJournalMilestoneRewardsRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "3a83iY3r"; }
	const char* GetAesKey() const override { return "98Tw0ubW"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
