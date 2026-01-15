#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class BuyCoinRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "9o3q7SjN"; }
	const char* GetAesKey() const override { return "DCM60QNb"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
