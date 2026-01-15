#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class PresentReceiptRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "bV5xa0ZW"; }
	const char* GetAesKey() const override { return "X2QFqAKfomPIg3rG"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
