#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class UnitEvoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "0gUSE84e"; }
	const char* GetAesKey() const override { return "biHf01DxcrPou5Qt"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
