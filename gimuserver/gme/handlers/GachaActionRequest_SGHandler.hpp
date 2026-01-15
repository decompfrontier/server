#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GachaActionRequest_SGHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "z3Di1k0e"; }
	const char* GetAesKey() const override { return "be51pQk9c"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
