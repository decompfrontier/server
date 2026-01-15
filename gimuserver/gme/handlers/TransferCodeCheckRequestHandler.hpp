#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class TransferCodeCheckRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Fn1UzTo3"; }
	const char* GetAesKey() const override { return "A6QiFT7D"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
