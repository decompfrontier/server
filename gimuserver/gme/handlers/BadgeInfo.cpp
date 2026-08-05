#include "App.hpp"
#include "Handlers.hpp"

HANDLEF(BadgeInfo)
{
	// Use BadgeInfoResp (the wrapper): it carries the "h23iRjGN" dispatch key the
	// client requires (see decompfrontier/server PR #23).
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
