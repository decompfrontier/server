#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FrontierGateStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "l3lkDBSc"; }
	const char* GetAesKey() const override { return "vYPCD34q"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
