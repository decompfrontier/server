#include "App.hpp"
#include "Handlers.hpp"

// ChallengeArenaResetInfo — "Zw3WIoWu" / key "KlwYMGF1"
//
// Spam loop (before this handler was registered):
// 1. How it starts:
//    Parent scene calls createConnectSceneToHome() (0xE9B6A0), which creates a
//    ChallengeArenaResetInfoConnectScene. On activation, initConnect() (0xE9B7A8)
//    calls needServerRefresh() (0xE7FA00). Since all internal fields are zero,
//    this always returns true, triggering accessPhpChallengeArena() (0x16082E0).
//
// 2. Why no handler caused infinite retry:
//    The server hit default: in getHandler(), returning GmeError{cmd=Close}.
//    checkConnectResult() (0xE9B828) saw the error and returned false.
//    updateEvent() never reached changeScene(), so the connect scene never
//    transitioned away. The game loop kept calling initConnect() on this
//    still-alive scene, producing an endless request stream at ~50ms intervals.
//
// 3. Why changeScene breaks the loop:
//    This is a transient bridge scene. Its only job is to fetch the response,
//    then hand off to a permanent arena scene via changeScene() and self-destruct.
//    When the server returned Close, the transition never happened, so the scene
//    stayed alive forever. error -4000 in noticeOK() (0xE9B8A8) is the escape
//    hatch that force-transitions without checking response validity, but our
//    Close error did not map to -4000.
//
// 4. Future retriggers:
//    Once a response populates the timestamp fields, needServerRefresh() will
//    eventually return true again after enough time passes. At that point the
//    parent scene recreates the bridge scene, which sends a single request.
//    This is normal behavior, not a spam loop.

HANDLEF(ChallengeArenaResetInfo)
{
	// Currently returns empty timestamps — client retries ~every 1s until real values are provided.
	// TODO: populate with real timestamps
	::ChallengeArenaResetInfoResp resp{};

	std::string buffer{};
	const auto& ec = glz::write_json(resp, buffer);
	if (ec) {
		co_return HandleResult::error("Serialization error", glz::format_error(ec, buffer));
	}

	co_return HandleResult::success(buffer);
}