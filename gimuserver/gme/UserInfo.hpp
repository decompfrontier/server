#pragma once

#include "response/UserInfo.hpp"
#include "response/UserTeamInfo.hpp"

constexpr const auto HARDCODE_USERID = "AAAAAAAA";

struct UserInfo
{
	Response::UserInfo info;
	Response::UserTeamInfo teamInfo;
};
