#include "App.hpp"
#include "Handlers.hpp"

HANDLEF(ControlCenterEnter)
{
	const auto& resp = theServer()->cache().braveSlotsResp();
	std::string buffer{};
	const auto& ec = glz::write_json(resp, buffer);
	if (ec)
	{
		LOG_DEBUG << "Cannot serialize ControlCenterEnter: " << ec;
		co_return "";
	}

	co_return buffer;
}
