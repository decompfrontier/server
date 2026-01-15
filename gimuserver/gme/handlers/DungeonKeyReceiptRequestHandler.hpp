#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class DungeonKeyReceiptRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "WCJE0xe2"; }
	const char* GetAesKey() const override { return "V4pfQo5C"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
