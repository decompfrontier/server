#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ArenaInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "oim9TU1D"; }
	const char* GetAesKey() const override { return "oqQxdFfa"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
