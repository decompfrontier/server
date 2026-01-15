#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GetPlayerInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "vUQrAV65"; }
	const char* GetAesKey() const override { return "7pW4xF9H"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
