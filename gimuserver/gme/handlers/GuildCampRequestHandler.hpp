#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildCampRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "38b67ie1"; }
	const char* GetAesKey() const override { return "hG738a5b"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
