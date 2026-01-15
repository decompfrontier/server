#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class RaidFrlendSelectRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "u59zdKCw"; }
	const char* GetAesKey() const override { return "0HJ9ShKg"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
