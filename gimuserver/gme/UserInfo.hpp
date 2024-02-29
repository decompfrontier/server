#pragma once

#include "response/UserInfo.hpp"
#include "response/UserTeamInfo.hpp"

constexpr const auto HARDCODE_USERID = "0101AABB";

struct UserInfo
{
	Response::UserInfo info;
	Response::UserTeamInfo teamInfo;
};
