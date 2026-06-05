#include "App.hpp"
#include "Handlers.hpp"
#include "common.hpp"

#include <gimuserver/db/PacketInterface.hpp>
#include <gimuserver/db/UserInfoService.hpp>

HANDLEF(UserInfo)
{
	UserInfoReq req = {};
	const auto& ec = glz::read_json(req, json);
	if (ec)
	{
		const auto& fmte = glz::format_error(ec, json);
		LOG_DEBUG << "Gme UserInfo Error during JSON read: " << fmte;
		co_return HandleResult::error("Deserialization error", fmte);
	}

    // Copy the cached response and build on top of it.
	UserInfoResp resp = theServer()->cache().userInfoResp();

	// TODO: A real server should check if user_id == gumi token...

    const auto db = theDb();
	std::string gumiUserId = req.login_info.gumi_live_userid;
	if (gumiUserId.empty())
	{
		LOG_ERROR << "UserInfo request did not include gumi_live_userid. "
			"We cannot proceed without this information.";
		co_return HandleResult::error("Missing gumi_live_userid", "UserInfo request did not include gumi_live_userid");
	}

	// If the client sent us an explicit user id, we should verify that it matches
	// what we have in the database.
	CO_AWAIT_DB(gme::nonEmpty(UserInfoService::fetchUserForGumiUser(
		theDb(),
		gumiUserId,
		resp.login_info.user_id)));
	if (!req.login_info.user_id.empty() && resp.login_info.user_id != req.login_info.user_id)
	{
		LOG_ERROR << "User ID mismatch for Gumi Live user " << gumiUserId
			<< ": client sent " << req.login_info.user_id
			<< ", database has " << resp.login_info.user_id;
		co_return HandleResult::error("User ID mismatch", "User ID mismatch for Gumi Live user " + std::string(gumiUserId));
	}

    // We must have a valid user entry in the database at this point.
	CO_AWAIT_DB(gme::nonEmpty(PacketInterfaceFor<LoginInfoResp>::readToPacket(
        db,
        "userinfo",
        {
            { "gumi_user_id", gumiUserId },
            { "id", resp.login_info.user_id },
        },
        resp.login_info)));
    resp.login_info.tutorial_end_flag = gme::getTutorialEndFlag(resp.login_info.tutorial_status);

	CO_AWAIT_DB(gme::nonEmpty(PacketInterfaceFor<UserTeamInfo>::readToPacket(
		db,
		"userinfo",
		{
			{ "gumi_user_id", gumiUserId },
			{ "id", resp.login_info.user_id },
		},
		resp.team_info)));
    resp.team_info.reinforcement_deck.emplace_back(0);
    resp.team_info.reinforcement_deck.emplace_back(0);
    resp.team_info.reinforcement_deck.emplace_back(0);
    resp.team_info.exp = 0;
    resp.team_info.add_unit_count = 100;
    resp.team_info.deck_cost = gme::deckCostForLevel(resp.team_info.level);

    // We should always have at least one unit, since the game will not let users
    // delete their only unit on the squad.
	CO_AWAIT_DB(gme::nonEmpty(PacketInterfaceFor<UserUnitInfo>::readToPackets(
        db,
        "user_units",
        { { "user_id", resp.login_info.user_id } },
        resp.unit_info)));

    const auto activeUserUnitId = resp.unit_info.front().user_unit_id;
    for (int i = 0; i < 10; i++) {
        UserPartyDeckInfo deck = {};
        deck.deck_num = i;
        deck.deck_type = 1;
        deck.user_unit_id = activeUserUnitId;
        resp.party_deck_info.emplace_back(deck);
    }

    resp.campaign_info.current_day = 1;
    resp.campaign_info.total_days = 96;
    resp.campaign_info.first_for_the_day = true;
    resp.campaign_info.id = 1;

    resp.summoner_journal.user_id = resp.login_info.user_id;
    resp.signal_key.key = "5EdKHavF";


    std::string buffer{};
    const auto& ec2 = glz::write_json(resp, buffer);
    if (ec2)
    {
        const auto& glze = glz::format_error(ec2, buffer);
        LOG_DEBUG << "Gme UserInfo Error during JSON writing: " << glze;
        co_return HandleResult::error("Serialization error", glze);
    }

    co_return HandleResult::success(buffer);
}
