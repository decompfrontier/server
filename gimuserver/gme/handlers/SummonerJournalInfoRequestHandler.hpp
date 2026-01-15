#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class SummonerJournalInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "32Gwida0"; }
	const char* GetAesKey() const override { return "66B2pDki"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
