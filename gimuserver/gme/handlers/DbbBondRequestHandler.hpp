#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class DbbBondRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "0EtanubR"; }
	const char* GetAesKey() const override { return "Tr7dR4dR"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
