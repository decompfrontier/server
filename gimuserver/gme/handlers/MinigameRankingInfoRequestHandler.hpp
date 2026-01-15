#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class MinigameRankingInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "XrfDHUJo"; }
	const char* GetAesKey() const override { return "cw9sw5y2"; }
	// TODO: Response has 4 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
