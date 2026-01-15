#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class VideoAdCompleteRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "r234ydi1"; }
	const char* GetAesKey() const override { return "mDqi310y"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
