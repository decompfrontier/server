#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class MysteryBoxClaimRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "2paswUpR"; }
	const char* GetAesKey() const override { return "kadRadU5"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
