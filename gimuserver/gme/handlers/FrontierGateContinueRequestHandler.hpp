#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FrontierGateContinueRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "uiFIMUH6"; }
	const char* GetAesKey() const override { return "ZiosS4cd"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
