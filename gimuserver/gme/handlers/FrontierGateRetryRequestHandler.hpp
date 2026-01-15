#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FrontierGateRetryRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "9pzHMBzq"; }
	const char* GetAesKey() const override { return "njsKMqGT"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
