#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FriendRandomSearchRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "mE5gUQOp"; }
	const char* GetAesKey() const override { return "oK4VaDGF"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
