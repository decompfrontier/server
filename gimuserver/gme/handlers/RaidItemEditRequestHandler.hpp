#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidItemEditRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "DZLI83hx"; }
	const char* GetAesKey() const override { return "3Lxpf5KE"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
