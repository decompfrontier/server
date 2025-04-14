#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class UpdateInfoLightHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "ynB7X5P9"; }
	const char* GetAesKey() const override { return "7kH9NXwC"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
