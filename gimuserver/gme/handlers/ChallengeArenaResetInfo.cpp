#include "App.hpp"
#include "Handlers.hpp"

// ChallengeArenaResetInfo
//
// By default the server returns GmeError{cmd=Close}, which the
// client interpreted as a connection failure. The connect scene never
// transitioned away, so the game loop re-fired the request every frame.
//
// With this handler returning empty timestamps (instead of an error), the client parses
// a valid response and stores server_time + a local snapshot. needServerRefresh() computes:
//
//     server_time + now() - local_snapshot > daily_cooling_end
//
// Assume fields start at zero. After parsing the empty response, server_time stays 0,
// local_snapshot is set to the current time (via time()), and daily_cooling_end stays 0.
// So the check becomes: 0 + now() - local_snapshot > 0.
//
// Since local_snapshot == the time() value from just after parsing, this is false until
// now() ticks forward to the next second.
//
// To stop the client from retrying every second, we'll set daily_cooling_end to an hour in the
// future.
//
// TODO: figure out the actual intended cooldown time and use that instead.
HANDLEF(ChallengeArenaResetInfo)
{
	using namespace std::chrono;
	using namespace std::chrono_literals;

	::ChallengeArenaResetInfoResp resp{};

	// I've tested this with 5 seconds as well, which causes the client to retry every 6 seconds.
	// This makes sense since its an explicit greater-than check.
	// Using an hour here as a placeholder to prevent the client from constantly retrying and polluting
	// our logs (and our disks :D).
	resp.reset_info.server_time = floor<milliseconds>(system_clock::now());
	resp.reset_info.daily_cooling_end = resp.reset_info.server_time + 1h;

	std::string buffer{};
	const auto& ec = glz::write_json(resp, buffer);
	if (ec) {
		const auto& glze = glz::format_error(ec, buffer);
		LOG_DEBUG << "Gme ChallengeArenaResetInfo Error during JSON writing: " << glze;
		co_return HandleResult::error("Serialization error", glze);
	}

	co_return HandleResult::success(buffer);
}