#pragma once

#include "response/UserInfo.hpp"
#include "response/UserTeamInfo.hpp"

constexpr const auto HARDCODE_USERID = "0101AABB";

//TODO: Is this still a required class?

struct UserInfo
{
	Response::UserInfo info;
	Response::UserTeamInfo teamInfo;
};
