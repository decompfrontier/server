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
		LOG_DEBUG << "Cannot serialize BadgeInfo: " << ec;
		co_return "";
	}

	co_return buffer;
}
