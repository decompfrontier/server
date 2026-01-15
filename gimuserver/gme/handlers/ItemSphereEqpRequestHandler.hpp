#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ItemSphereEqpRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "0IXGiC9t"; }
	const char* GetAesKey() const override { return "CZE56XAY"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
