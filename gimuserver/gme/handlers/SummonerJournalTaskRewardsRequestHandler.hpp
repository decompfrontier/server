#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class SummonerJournalTaskRewardsRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "2y48D13d"; }
	const char* GetAesKey() const override { return "7nm3Dqe9"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
