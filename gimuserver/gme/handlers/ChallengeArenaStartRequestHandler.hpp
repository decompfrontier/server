#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ChallengeArenaStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "z44LyLr2"; }
	const char* GetAesKey() const override { return "CC8rm3EJ"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
