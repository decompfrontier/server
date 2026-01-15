#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class VortexArenaLeaderboardRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "290aD3iY"; }
	const char* GetAesKey() const override { return "d2l0Bdi8"; }
	// TODO: Response has 11 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
