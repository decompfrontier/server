#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class FriendGetHandler : public IHandler
{
public:
	const char* GetAesKey() const override { return "EoYuZ2nbImhCU1c0"; }
	bool Handle(const Json::Value& req, Json::Value& res) override;
};
HANDLER_NS_END
