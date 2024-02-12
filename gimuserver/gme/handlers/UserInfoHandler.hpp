#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
//id: cTZ3W2JG
class UserInfoHandler : public HandlerBase
{
public:
	const char* GetAesKey() const override { return "ScJx6ywWEb0A3njT"; }
	void Handle(const Json::Value& req) override;
};
HANDLER_NS_END
