#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class SPChallengeRankingRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "27rGx1rL"; }
	const char* GetAesKey() const override { return "pTDy82st"; }
	// TODO: Response has 10 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
