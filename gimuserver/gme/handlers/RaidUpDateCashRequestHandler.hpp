#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidUpDateCashRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "XTban70i"; }
	const char* GetAesKey() const override { return "H5d6B2pi"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
