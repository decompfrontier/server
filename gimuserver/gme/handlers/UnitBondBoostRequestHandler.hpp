#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class UnitBondBoostRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "tr5rOwro"; }
	const char* GetAesKey() const override { return "fus9A2ut"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
