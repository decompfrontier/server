#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FrontierGateInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "M17pPotk"; }
	const char* GetAesKey() const override { return "sochkegz"; }
	// TODO: Response has 11 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
