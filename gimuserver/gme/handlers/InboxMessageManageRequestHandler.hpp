#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class InboxMessageManageRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "rYSfaC4P"; }
	const char* GetAesKey() const override { return "0R9ZPaSf"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
