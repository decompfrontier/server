#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class MissionRestartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "IP96ys7T"; }
	const char* GetAesKey() const override { return "0Zy3G9eD"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
