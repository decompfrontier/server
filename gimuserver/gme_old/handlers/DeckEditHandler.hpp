#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class DeckEditHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "m2Ve9PkJ"; }
	const char* GetAesKey() const override { return "d7UuQsq8"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
