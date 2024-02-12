#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
//id: MfZyu1q9
class InitializeHandler : public HandlerBase
{
public:
	const char* GetAesKey() const override { return "EmcshnQoDr20TZz1"; }
	void Handle(const Json::Value& res) override;
};
HANDLER_NS_END
