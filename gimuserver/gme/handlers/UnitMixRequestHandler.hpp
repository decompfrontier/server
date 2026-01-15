#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class UnitMixRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "Mw08CIg2"; }
	const char* GetAesKey() const override { return "JnegC7RrN3FoW8dQ"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
