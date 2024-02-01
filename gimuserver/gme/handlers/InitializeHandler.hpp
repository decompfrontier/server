#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
//id: MfZyu1q9
class InitializeHandler : public IHandler
{
public:
	const char* GetAesKey() const override { return "EmcshnQoDr20TZz1"; }
	bool Handle(const Json::Value& req, Json::Value& res) override;
};
HANDLER_NS_END
