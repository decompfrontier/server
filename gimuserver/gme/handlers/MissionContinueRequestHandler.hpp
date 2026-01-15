#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class MissionContinueRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "p8B2i9rJ"; }
	const char* GetAesKey() const override { return "G3FwvQfy5hcxHMen"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
