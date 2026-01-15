#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidUpdateProgressRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "7geDL5Vb"; }
	const char* GetAesKey() const override { return "7DkVMG8Y"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
