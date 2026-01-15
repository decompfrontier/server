#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class CollabRewardRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "4aL5upiU"; }
	const char* GetAesKey() const override { return "2a4MeshE"; }
	// TODO: Response has 2 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
