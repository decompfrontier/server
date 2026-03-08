#pragma once

#include "../GmeHandler.hpp"

HANDLER_NS_BEGIN
class GetUserInfoRequestHandler : public HandlerBase
{
public:
	const char* GetGroupId() const override { return "cTZ3W2JG"; }
	const char* GetAesKey() const override { return "ScJx6ywWEb0A3njT"; }
	void Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const override;

private:
	// Second-phase dispatch called after user ID has been resolved from the users table.
	// Exists because the Windows client sends dummy values like '0101AABB' as user_id
	// in the encrypted request body, so Handle() first resolves the real ID async,
	// then delegates here where all member functions (GetGroupId, GetAesKey, OnError)
	// are in scope normally.
	void HandleResolved(UserInfo& user, DrogonCallback cb, const Json::Value& req) const;
};
HANDLER_NS_END