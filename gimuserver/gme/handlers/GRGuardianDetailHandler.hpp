#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GRGuardianDetailHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "5710Dkla"; }
	const char* GetAesKey() const override { return "f89a2kd7"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
