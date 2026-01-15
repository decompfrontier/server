#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FriendAgreeRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "NYKBH4V6"; }
	const char* GetAesKey() const override { return "8PFVDiUIZyES2vzR"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
