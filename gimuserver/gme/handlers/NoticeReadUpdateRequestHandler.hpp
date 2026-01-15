#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class NoticeReadUpdateRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "cuKwx5rF"; }
	const char* GetAesKey() const override { return "o2rhxCmg"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
