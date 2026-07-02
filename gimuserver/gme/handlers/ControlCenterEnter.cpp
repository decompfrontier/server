#include "App.hpp"
#include "Handlers.hpp"

HANDLEF(ControlCenterEnter)
{
	const auto& resp = theServer()->cache().braveSlotsResp();
	std::string buffer{};
	const auto& ec = glz::write_json(resp, buffer);
	if (ec)
	{
		const auto& glze = glz::format_error(ec, buffer);
		LOG_DEBUG << "Gme ControlCenterEnter Error during JSON writing: " << glze;
		co_return HandleResult::error("Serialization error", glze);
	}

	co_return HandleResult::success(buffer);
}
