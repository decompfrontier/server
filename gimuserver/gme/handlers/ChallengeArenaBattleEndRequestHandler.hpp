#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ChallengeArenaBattleEndRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "syX20UG8"; }
	const char* GetAesKey() const override { return "9xk0xi0Y"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
