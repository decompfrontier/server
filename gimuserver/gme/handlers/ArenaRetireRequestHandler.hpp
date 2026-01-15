#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ArenaRetireRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "eyg8sA32"; }
	const char* GetAesKey() const override { return "93i8EbyM"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
