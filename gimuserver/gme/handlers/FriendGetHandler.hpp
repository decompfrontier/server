#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FriendGetHandler : public HandlerBase
{
public:
	const char* GetAesKey() const override { return "EoYuZ2nbImhCU1c0"; }
	void Handle(const Json::Value& req) override;
};
HANDLER_NS_END
