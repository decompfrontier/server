#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ChronologyRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "5o8ZlDGX"; }
	const char* GetAesKey() const override { return "SNrhAG29"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
