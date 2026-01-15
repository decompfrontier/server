#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidWorloInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "H7GRp01L"; }
	const char* GetAesKey() const override { return "5JxFp0tD"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
