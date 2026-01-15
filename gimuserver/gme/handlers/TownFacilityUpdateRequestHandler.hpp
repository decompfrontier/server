#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class TownFacilityUpdateRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "8v43tz7g"; }
	const char* GetAesKey() const override { return "rq7Yd1nG"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
