#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/utils/Random.hpp>

HANDLEF(Initialize)
{
	InitializeReq req = {};
	// Use lenient parsing so extra envelope keys from the client (e.g. device
	// metadata) don't cause a spurious parse failure.
	if (const auto ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json); ec)
	{
		const auto fmte = glz::format_error(ec, json);
		LOG_DEBUG << "Gme Initialize Error during JSON read: " << fmte;
		co_return HandleResult::error("Deserialization error", fmte);
	}

	// NOTE: A real server would verify the gumi token first...
	// TODO: Handle MSTs to answer

	InitializeResp resp = theServer()->cache().initializeResp(); // copy !!

	const auto& userRows = co_await theDb()->execSqlCoro(
		"SELECT username, debug_mode FROM userinfo WHERE id=$1;",
		std::string("0839899613932562")
	);

	// Guard against an empty result — can happen on a fresh or partially-migrated
	// DB.  Return a successful response with a default handle name so the client
	// can at least get past the login screen; the row will be created by the
	// migration on the next server start.
	if (userRows.empty())
	{
		LOG_WARN << "Initialize: no userinfo row for '0839899613932562' — responding with defaults";
		resp.login_info.account_id        = "0839899613932562";
		resp.login_info.user_id           = "0839899613932562";
		resp.login_info.handle_name       = "DecompDev";
		resp.login_info.debug_mode        = false;
		resp.login_info.tutorial_end_flag  = true;
		resp.login_info.tutorial_status    = 12;
		resp.signal_key.key               = "C7vnXA5T";
		resp.challenge_arena_user_info.user_id = "n9ZMPC0t";
		resp.challenge_arena_user_info.unkstr2 = "F";
		resp.challenge_arena_user_info.league_id = 1;
		resp.summoner_journal.user_id     = "0839899613932562";
		resp.daily_login_rewards.id       = 1;
		resp.daily_login_rewards.current_day = 1;
		resp.daily_login_rewards.message  = " day(s) more to guaranteed Gem!";
		std::string buf{};
		if (const auto ec2 = glz::write_json(resp, buf); ec2)
			co_return HandleResult::success("{}");
		co_return HandleResult::success(buf);
	}

	const auto& userRow = userRows.at(0);

	resp.login_info.account_id       = "0839899613932562";
	resp.login_info.user_id          = "0839899613932562"; // packet-gen UUID
	resp.login_info.handle_name      = userRow["username"].as<std::string>();
	resp.login_info.debug_mode       = userRow["debug_mode"].as<bool>();
	resp.login_info.tutorial_end_flag = true;
	resp.login_info.tutorial_status   = 12;

	//resp.user_info.gumi_live_token = req.user_info.gumi_live_token;
	//resp.user_info.gumi_live_userid = req.user_info.gumi_live_userid;

	resp.signal_key.key = "C7vnXA5T";

	resp.challenge_arena_user_info.user_id = "n9ZMPC0t"; // rank name?
	resp.challenge_arena_user_info.unkstr2 = "F"; // ranking?
	resp.challenge_arena_user_info.league_id = 1;

	resp.summoner_journal.user_id = resp.login_info.user_id;

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

