
/*
    This file was autogenerated by decompfrontier packet-generator.
    Generation date: 2025-04-10 17:17:19

    DO NOT MODIFY THIS FILE! MODIFY THE FILE ./data/net/badgeinfo.py INSIDE THE
    PACKET GENERATOR REPOSITORY AND RUN THE GENERATION AGAIN!
*/
#pragma once

#include <glaze_helpers.hpp> // c++ runtime helpers

struct BadgeInfo {
	std::string	badge_data{};
	uint32_t	dungeon_key_num{};
	uint32_t	item_dict_num{};
	uint32_t	new_frohun{};
	uint32_t	scenario_num{};
	uint32_t	unit_dict_num{};
};
template <>
struct glz::meta<BadgeInfo>
{
    using T = BadgeInfo;
    static constexpr auto value = object(
		"dUujjBBK", &T::badge_data,
		"dD40xrsh", &T::dungeon_key_num,
		"iakSd6m8", &T::item_dict_num,
		"4AjsNu0m", &T::new_frohun,
		"u7Htm9P4", &T::scenario_num,
		"yCLEs28F", &T::unit_dict_num
    );
};




