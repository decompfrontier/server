#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ArenaMatchingRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "82ztj6Iy"; }
	const char* GetAesKey() const override { return "Q53tUIBE"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
