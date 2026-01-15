#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class VideoAdSlotsStandInfoRefreshRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "jwgJVpUP"; }
	const char* GetAesKey() const override { return "sjrlKA3D"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
