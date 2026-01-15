#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class UpdateInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "RUV94Dqz"; }
	const char* GetAesKey() const override { return "hy0P9xjsGJ6MAgb2"; }
	// TODO: Response has 1 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
