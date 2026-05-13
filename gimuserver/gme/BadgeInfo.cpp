#include "App.hpp"
#include "Handlers.hpp"

HANDLEF(BadgeInfo)
{
	// IMPORTANT: must use BadgeInfoResp (the wrapper), NOT BadgeInfo (the inner struct).
	// The wrapper carries the "h23iRjGN" dispatch key that the client's response
	// pipeline looks for; sending the inner BadgeInfo directly produces "u7Htm9P4"
	// at the root and the client silently drops the packet.  Discovered upstream
	// in decompfrontier/server PR #23.
	::BadgeInfoResp resp{};
	std::string buffer{};
	const auto& ec = glz::write_json(resp, buffer);
	if (ec)
	{
		const auto& glze = glz::format_error(ec, buffer);
		LOG_DEBUG << "Gme BadgeInfo Error during JSON writing: " << glze;
		co_return HandleResult::error("Serialization error", glze);
	}

	co_return HandleResult::success(buffer);
}
