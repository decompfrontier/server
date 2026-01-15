#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class VortexArenaRetireRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "R34vyqw7"; }
	const char* GetAesKey() const override { return "j8M3a93W"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
