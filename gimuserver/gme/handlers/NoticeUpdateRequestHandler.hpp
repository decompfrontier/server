#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class NoticeUpdateRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "68pTQAJv"; }
	const char* GetAesKey() const override { return "WHfcd53M"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
