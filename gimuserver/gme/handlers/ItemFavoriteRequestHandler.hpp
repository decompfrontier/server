#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ItemFavoriteRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "I8il6EiI"; }
	const char* GetAesKey() const override { return "aRoIftRy"; }
	// TODO: Response has 2 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
