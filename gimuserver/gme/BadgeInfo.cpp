#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/packets/net_badgeinfo.hpp>

HANDLEF(BadgeInfo)
{
	::BadgeInfo resp{};
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
