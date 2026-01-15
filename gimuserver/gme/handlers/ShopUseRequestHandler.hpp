#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ShopUseRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "xe8tiSf4"; }
	const char* GetAesKey() const override { return "qthMXTQSkz3KfH9R"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
