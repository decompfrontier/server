#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FriendFavoriteRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "ps6Vgx4R"; }
	const char* GetAesKey() const override { return "XcE1CK6L"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
