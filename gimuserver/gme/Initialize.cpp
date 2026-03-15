#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/utils/Random.hpp>

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

	InitializeResp resp = theServer()->cache().initializeResp(); // copy !!

#if 0
	const auto& res = co_await theDb()->execSqlCoro("SELECT id, username, debug_mode FROM userinfo WHERE gumi_user_id=$1", req.login_info.gumi_live_userid);
	if (res.empty())
	{
		// Gumi user does not exist! Create a new user and add it to the database

		const auto& cache = theServer()->cache();
		const auto& scfg = cache.serverConfig();
		const auto& def = cache.initializeResp().defines;

		// No handle! We are a new user after all!
		resp.login_info.account_id = "1111";

		co_await theDb()->execSqlCoro("INSERT INTO userinfo (id, gumi_user_id, device_id, debug_mode, "
			"level, "
			"max_unit_count, max_friend_count, "
			"zel, karma, brave_coin, "
			"max_warehouse_count, free_gems, energy) "
			"VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13);",
			// id, gumi_user_id, device_id, debug_mode
			resp.login_info.account_id, req.login_info.gumi_live_userid, req.login_info.device_id, false,
			// level
			scfg.initialLevel,
			// max_unit_count, max_friend_count
			50, 200,
			// zel, karma, brave_coin
			scfg.initialZel, scfg.initialKarma, scfg.initialBraveCoins,
			// max_warehouse, free_gems, energy
			10, 5, 20);
	}
	else
	{
		// only one query pls
		const auto& sql = res[0];
		size_t col = 0;
		resp.login_info.account_id = sql[col++].as<std::string>();
		resp.login_info.handle_name = sql[col++].as<std::string>();
		resp.login_info.debug_mode = sql[col++].as<bool>();
	}
#endif


	// TODO: GET THIS FROM A CACHE TOKEN ETC
	resp.login_info.account_id = "12345678";
	resp.login_info.handle_name = "OfflineMod!";
	resp.login_info.user_id = "0839899613932562"; // I think this is a random UUID according to packet-gen
	// TEMP HACK!! Skip tutorial flag and put a real name
	resp.login_info.tutorial_end_flag = true;
	resp.login_info.tutorial_status = 12;
	resp.login_info.feature_gate = "0";

	//resp.user_info.gumi_live_token = req.user_info.gumi_live_token;
	//resp.user_info.gumi_live_userid = req.user_info.gumi_live_userid;

	resp.signal_key.key = "C7vnXA5T";

	resp.challenge_arena_user_info.unk = "n9ZMPC0t"; // rank name?
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

