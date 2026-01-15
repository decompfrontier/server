#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GuildRecomendedMemberRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "ja5Enusw"; }
	const char* GetAesKey() const override { return "8upheqaC"; }
	// TODO: Response has 7 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
