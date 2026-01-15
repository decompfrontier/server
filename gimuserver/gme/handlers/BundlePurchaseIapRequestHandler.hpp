#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class BundlePurchaseIapRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "kD298bt6"; }
	const char* GetAesKey() const override { return "u45Bi2p0"; }
	// TODO: Response has 5 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
