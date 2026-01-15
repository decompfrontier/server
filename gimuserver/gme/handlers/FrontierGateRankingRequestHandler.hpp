#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FrontierGateRankingRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "26zW90oG"; }
	const char* GetAesKey() const override { return "yo9eBUJk"; }
	// TODO: Response has 16 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
