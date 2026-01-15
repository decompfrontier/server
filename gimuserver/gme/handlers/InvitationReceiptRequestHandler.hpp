#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class InvitationReceiptRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "7Zxr9PzB"; }
	const char* GetAesKey() const override { return "7s5Ic3Nx"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
