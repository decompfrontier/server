
/*
    This file was autogenerated by decompfrontier packet-generator.
    Generation date: 2025-04-10 17:17:19

    DO NOT MODIFY THIS FILE! MODIFY THE FILE ./data/mst/events.py INSIDE THE
    PACKET GENERATOR REPOSITORY AND RUN THE GENERATION AGAIN!
*/
#pragma once

#include <glaze_helpers.hpp> // c++ runtime helpers

struct GeneralEventMstData {
};
template <>
struct glz::meta<GeneralEventMstData>
{
    using T = GeneralEventMstData;
    static constexpr auto value = object
    );
};

struct GeneralEventMst {
    using TYPE = GeneralEventMstData;
    std::deque<TYPE> data;
};
template <>
struct glz::meta<GeneralEventMst>
{
    using T = GeneralEventMst;
    static constexpr auto value = object("Md9gG3c0", &T::data);
};

