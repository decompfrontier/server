#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
//id: cTZ3W2JG
class UserInfoHandler : public IHandler
{
public:
	const char* GetAesKey() const override { return "ScJx6ywWEb0A3njT"; }
	bool Handle(const Json::Value& req, Json::Value& res) override;
};
HANDLER_NS_END
