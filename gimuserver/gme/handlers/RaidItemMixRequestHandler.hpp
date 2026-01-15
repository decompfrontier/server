#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidItemMixRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "K17JUYR2"; }
	const char* GetAesKey() const override { return "c6wBM9sH"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
