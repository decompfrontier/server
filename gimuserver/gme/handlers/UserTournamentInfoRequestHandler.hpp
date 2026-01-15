#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class UserTournamentInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "L2kdaTx9"; }
	const char* GetAesKey() const override { return "10db8akS5kaZ32lM"; }
	// TODO: Response has 5 parameters to implement
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
