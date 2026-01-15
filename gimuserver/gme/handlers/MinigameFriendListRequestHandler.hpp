#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class MinigameFriendListRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Hjk84giw"; }
	const char* GetAesKey() const override { return "9qg5s7ob"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
