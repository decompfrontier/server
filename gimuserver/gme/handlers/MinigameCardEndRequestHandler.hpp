#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class MinigameCardEndRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "jJKzi0v2"; }
	const char* GetAesKey() const override { return "eHt7ob1v"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
