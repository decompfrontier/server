#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidMissionSelectRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "8iQL3IKA"; }
	const char* GetAesKey() const override { return "ZBex5ju9"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
