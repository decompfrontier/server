#include "App.hpp"
#include "Handlers.hpp"

HANDLEF(GatchaList)
{
	GatchaListReq req = {};
	const auto& ec = glz::read_json(req, json);
	if (ec)
	{
		const auto& fmte = glz::format_error(ec, json);
		LOG_DEBUG << "Gme GatchaList Error during JSON read: " << fmte;
		co_return HandleResult::error("Deserialization error", fmte);
	}

	// Just reuse the static response that we've cached.
	GatchaListResp resp = theServer()->cache().gatchaListRsp();
	resp.signal_key = req.signal_key;

	std::string buffer;
	const auto& ec2 = glz::write_json(resp, buffer);
	if (ec2)
	{
		const auto& glze = glz::format_error(ec2, buffer);
		LOG_DEBUG << "Gme GatchaList Error during JSON writing: " << glze;
		co_return HandleResult::error("Serialization error", glze);
	}

	co_return HandleResult::success(buffer);
}
