#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GetGiftInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "ifYoPJ46"; }
	const char* GetAesKey() const override { return "6zHEYa9U"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
