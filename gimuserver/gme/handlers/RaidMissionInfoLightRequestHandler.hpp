#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidMissionInfoLightRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Argz1U6B"; }
	const char* GetAesKey() const override { return "1hJrYxs7"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
