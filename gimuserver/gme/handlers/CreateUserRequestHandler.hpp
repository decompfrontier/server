#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class CreateUserRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "uV6yH5MX"; }
	const char* GetAesKey() const override { return "4agnATy2DrJsWzQk"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
