#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FrontierGateEndRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "cAJp7U4l"; }
	const char* GetAesKey() const override { return "Vvpy7qZR"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
