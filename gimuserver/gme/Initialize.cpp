#include "App.hpp"
#include "Handlers.hpp"
#include "common.hpp"

#include <gimuserver/db/PacketInterface.hpp>
#include <gimuserver/db/UserInfoService.hpp>
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
	std::string gumiUserId = req.login_info.gumi_live_userid;
	if (gumiUserId.empty())
	{
		LOG_ERROR << "Initialize request did not include gumi_live_userid. "
			"We cannot proceed without this information.";
		co_return HandleResult::error("Missing gumi_live_userid", "Initialize request did not include gumi_live_userid");
	}

	// If the client sent us an explicit user id, we should verify that it matches
	// what we have in the database.
	CO_AWAIT_DB(UserInfoService::fetchUserForGumiUser(
		theDb(),
		gumiUserId,
		resp.login_info.user_id));
	if (!req.login_info.user_id.empty() && resp.login_info.user_id != req.login_info.user_id)
	{
		LOG_ERROR << "User ID mismatch for Gumi Live user " << gumiUserId
			<< ": client sent " << req.login_info.user_id
			<< ", database has " << resp.login_info.user_id;
		co_return HandleResult::error("User ID mismatch", "User ID mismatch for Gumi Live user " + std::string(gumiUserId));
	}

	// If we didn't find a user for this Gumi Live ID, we just return an empty user_id
	// and let the client enter the tutorial flow. Otherwise, we return the user info
	// stored in the database.
	if (!resp.login_info.user_id.empty())
	{
		CO_AWAIT_DB(gme::nonEmpty(PacketInterfaceFor<LoginInfoResp>::readToPacket(
			theDb(),
			"userinfo",
			{
				{ "gumi_user_id", gumiUserId },
				{ "id", resp.login_info.user_id },
			},
			resp.login_info)));
	}
	resp.login_info.tutorial_end_flag = gme::getTutorialEndFlag(resp.login_info.tutorial_status);

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

