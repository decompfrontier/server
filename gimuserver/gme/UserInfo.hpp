#pragma once

#include "response/UserInfo.hpp"
#include "response/UserTeamInfo.hpp"

// NOTE: HARDCODE_USERID ("0101AABB") has been removed.
//
// It was a dead placeholder from before any real user data existed. The Windows
// client generates its user ID from device hardware (e.g. "0839899613932562")
// and sends it as the 'ak' query parameter in HandleGuest. That ID is what
// actually exists in the users/userinfo/user_units tables.
//
// User resolution now happens in GmeController::ResolveUser() at startup,
// which queries the DB and caches the real ID in m_resolvedUserID.
// No compile-time constant is needed or appropriate here.

struct UserInfo
{
    Response::UserInfo info;
    Response::UserTeamInfo teamInfo;
};