#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildUpdateRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "92bDoqBi"; }
	const char* GetAesKey() const override { return "w3Bne038"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
