#include "App.hpp"
#include "Handlers.hpp"
#include "Common.hpp"

#include <gimuserver/db/PacketInterface.hpp>

HANDLEF(Initialize)
{
	InitializeReq req = {};
	const auto& ec = glz::read_json(req, json);
	if (ec)
	{
		const auto& fmte = glz::format_error(ec, json);
		LOG_DEBUG << "Gme Initialize Error during JSON read: " << fmte;
		co_return HandleResult::error("Deserialization error", fmte);
	}

	// NOTE: A real server would verify the gumi token first...
	// TODO: Handle MSTs to answer

	// Copy the cached response and build on top of it.
	InitializeResp resp = theServer()->cache().initializeResp();

	// This is something to do with account transfer, ignore for now.
	resp.login_info.account_id = "12345678";
	// Assume we are a new user until proven otherwise. The game checks if these
	// values are empty to decide whether to enter the tutorial flow or not.
	resp.login_info.handle_name = "";
	resp.login_info.user_id = "";

	// After GuestLogin, Initialize is the first encrypted GME request the client
	// sends during normal startup. It receives the Gumi Live ID returned by the
	// account login flow and decides whether the client should resume an existing
	// game user or enter the new-user/tutorial flow.
	auto identity = (co_await gme::getUserIdentity(theDb(), req.login_info, true)).data;

	// If we didn't find a user for this Gumi Live ID, we just return an empty user_id
	// and let the client enter the tutorial flow. Otherwise, we return the user info
	// stored in the database.
	if (!identity.userId.empty())
	{
		resp.login_info = std::move((co_await gme::getLoginInfo(theDb(), identity)).nonEmpty());
	}

	//resp.user_info.gumi_live_token = req.user_info.gumi_live_token;
	//resp.user_info.gumi_live_userid = req.user_info.gumi_live_userid;

	resp.signal_key.key = "C7vnXA5T";

	resp.challenge_arena_user_info.user_id = "n9ZMPC0t"; // rank name?
	resp.challenge_arena_user_info.unkstr2 = "F"; // ranking?
	resp.challenge_arena_user_info.league_id = 1;

	resp.summoner_journal.user_id = identity.userId;

	resp.daily_login_rewards.id = 1;
	resp.daily_login_rewards.current_day = 1;
	resp.daily_login_rewards.message = " day(s) more to guaranteed Gem!";

	std::string buffer{};
	const auto& ec2 = glz::write_json(resp, buffer);
	if (ec2)
	{
		const auto& glze = glz::format_error(ec2, buffer);
		LOG_DEBUG << "Gme Initialize Error during JSON writing: " << glze;
		co_return HandleResult::error("Serialization error", glze);
	}

	co_return HandleResult::success(buffer);
}

