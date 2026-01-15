#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class UnitFavoriteRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "2p9LHCNh"; }
	const char* GetAesKey() const override { return "cb4ESLa1"; }
	// TODO: Response has 2 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
