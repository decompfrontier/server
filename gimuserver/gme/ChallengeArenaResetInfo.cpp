#include "App.hpp"
#include "Handlers.hpp"

#include <chrono>

// ChallengeArenaResetInfo (Zw3WIoWu / KlwYMGF1)
//
// Why this handler matters: by default (when unregistered) the server returns
// GmeError{cmd=Close}, which the client interprets as a connection failure.
// The connect-scene state machine never transitions away, so the game loop
// re-fires this request every frame (~50ms — observable in any TRACE log
// from before this handler was implemented as a wall of Zw3WIoWu requests).
//
// Returning a structurally-valid response with EMPTY timestamps fixes the
// 50ms cascade but introduces a 1-second one: the client's needServerRefresh()
// check is roughly
//
//     server_time + now() - local_snapshot > daily_cooling_end
//
// With all fields zero, the inequality reduces to `now() - local_snapshot > 0`,
// which becomes true as soon as the local time() value ticks forward by one
// second.  Net effect: ~1 req/s instead of ~20 req/s, but still spam.
//
// Returning a daily_cooling_end an HOUR in the future stops the cascade for
// 60 minutes — long enough that the request only fires when actually needed
// (re-entering the arena, server restart, etc.).
//
// Discovered upstream in decompfrontier/server PR #23 (Tom2096).
//
// TODO: derive the actual cooldown end from the configured arena reset
// schedule instead of using a fixed +1h offset.
HANDLEF(ChallengeArenaResetInfo)
{
    using namespace std::chrono;
    using namespace std::chrono_literals;

    ::ChallengeArenaResetInfoResp resp{};

    // server_time = real now (ms precision); daily_cooling_end = now + 1h.
    // weekly_reset_time + daily_cooling_start stay at epoch — the client
    // doesn't gate on those for this handler's polling logic.
    resp.reset_info.server_time       = floor<milliseconds>(system_clock::now());
    resp.reset_info.daily_cooling_end = resp.reset_info.server_time + 1h;

    std::string buffer{};
    if (const auto ec = glz::write_json(resp, buffer); ec)
    {
        const auto glze = glz::format_error(ec, buffer);
        LOG_DEBUG << "ChallengeArenaResetInfo: serialize error: " << glze;
        co_return HandleResult::error("Serialization error", glze);
    }

    co_return HandleResult::success(buffer);
}
