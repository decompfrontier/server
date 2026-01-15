#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ArenaBattleEndRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "oQNtKU51"; }
	const char* GetAesKey() const override { return "jkgWq1d0"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
