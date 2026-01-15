#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class BundlePurchaseRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "D3gyT3b3"; }
	const char* GetAesKey() const override { return "a8pE3x39"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
