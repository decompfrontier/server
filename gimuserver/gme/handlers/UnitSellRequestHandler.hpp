#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class UnitSellRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Ri3uTq9b"; }
	const char* GetAesKey() const override { return "92VqcGFWuPkmT60U"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
