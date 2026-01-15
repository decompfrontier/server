#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class UpdateEventInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "rCB7ZI8x"; }
	const char* GetAesKey() const override { return "L1o4eGbi"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
