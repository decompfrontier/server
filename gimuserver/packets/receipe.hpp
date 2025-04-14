
/*
    This file was autogenerated by decompfrontier packet-generator.
    Generation date: 2025-04-10 17:17:19

    DO NOT MODIFY THIS FILE! MODIFY THE FILE ./data/mst/receipe.py INSIDE THE
    PACKET GENERATOR REPOSITORY AND RUN THE GENERATION AGAIN!
*/
#pragma once

#include <glaze_helpers.hpp> // c++ runtime helpers

struct ReceipeMstData {
	uint32_t	id{};
	uint32_t	item_count{};
	uint32_t	item_id{};
	uint32_t	karma{};
	std::string	keyitem_ids{};
	std::string	materials{};
	std::string	unk{};
	uint32_t	unk2{};
	std::chrono::system_clock::time_point	unk3{};
	std::chrono::system_clock::time_point	unk4{};
};
template <>
struct glz::meta<ReceipeMstData>
{
    using T = ReceipeMstData;
    static constexpr auto value = object(
		"4HqhTf3a", &T::id,
		"wgV86x1q", &T::item_count,
		"kixHbe54", &T::item_id,
		"HTVh8a65", &T::karma,
		"7dEB1Kwj", &T::keyitem_ids,
		"75YV2q1i", &T::materials,
		"XuJL4pc5", &T::unk,
		"H6k1LIxC", &T::unk2,
		"qA7M9EjP", glzhlp::datetime<&T::unk3>,
		"SzV0Nps7", glzhlp::datetime<&T::unk4>
    );
};

struct ReceipeMst {
    using TYPE = ReceipeMstData;
    std::deque<TYPE> data;
};
template <>
struct glz::meta<ReceipeMst>
{
    using T = ReceipeMst;
    static constexpr auto value = object("8f0bCciN", &T::data);
};

