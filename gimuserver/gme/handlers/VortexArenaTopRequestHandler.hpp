#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class VortexArenaTopRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "lp3a7bvD"; }
	const char* GetAesKey() const override { return "38ab793G"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
