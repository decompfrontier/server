#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class InitializeHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "MfZyu1q9"; }
	const char* GetAesKey() const override { return "EmcshnQoDr20TZz1"; }
	void Handle(const drogon::SessionPtr& session, DrogonCallback& cb, const Json::Value& req) const override;
};
HANDLER_NS_END
