#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class VortexArenaClaimRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "iwo032Yj"; }
	const char* GetAesKey() const override { return "ab30d5H2"; }
	// TODO: Response has 1 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
