#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class ChallengeMissionStartRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "t5wfpZ32"; }
	const char* GetAesKey() const override { return "YSVD0n9K"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;
};
HANDLER_NS_END
