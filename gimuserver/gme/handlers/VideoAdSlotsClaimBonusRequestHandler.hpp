#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class VideoAdSlotsClaimBonusRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "29s22s49"; }
	const char* GetAesKey() const override { return "93055d2i"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
