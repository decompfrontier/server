#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ItemEditRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "ruoB7bD8"; }
	const char* GetAesKey() const override { return "DHEfRexCu0q5TAQm"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
