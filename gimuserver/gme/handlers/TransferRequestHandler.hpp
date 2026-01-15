#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class TransferRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "I9DyF2aU"; }
	const char* GetAesKey() const override { return "2hp90fZ1"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
