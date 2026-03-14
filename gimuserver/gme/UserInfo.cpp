#include "App.hpp"
#include "Handlers.hpp"

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

	UserInfoResp resp = theServer()->cache().userInfoResp(); // copy !!

	// TODO: A real server should check if user_id == gumi token...

    // TODO: GET THIS FROM A CACHE TOKEN ETC
    resp.login_info.user_id = "0000AAAA"; // I think this is a random UUID according to packet-gen

    // TEMP HACK!! Skip tutorial flag and put a real name
    resp.login_info.account_id = "1111";
    resp.login_info.debug_mode = false;
    resp.login_info.handle_name = "OfflineMod!";
    resp.login_info.tutorial_end_flag = true;
    resp.login_info.tutorial_status = 13;
    resp.login_info.unk = "773c9af44721a014c7ed";

    UserTeamInfo team = {};
    team.user_id = req.login_info.user_id;
    team.level = 1;
    team.exp = 0;
    team.max_unit_count = 4000; // Updated to match unit count
    team.max_friend_count = 100;
    team.zel = 10000000;
    team.karma = 10000000;
    team.brave_coin = 0;
    team.warehouse_count = 4000; // Updated to match unit count
    team.free_gems = 99999;
    team.paid_gems = 0;
    team.summon_ticket = 0;
    team.colosseum_ticket = 0;
    team.deck_cost = 100;
    team.max_friend_count = 100;
    team.add_friend_count = 100;
    team.max_action_point = 5000;
    team.action_point = 5000;
    team.reinforcement_deck.emplace_back(0);
    team.reinforcement_deck.emplace_back(0);
    team.reinforcement_deck.emplace_back(0);
    resp.team_info = team;

    UserUnitInfo d = {};
    d.user_id = req.login_info.user_id;
    d.user_unit_id = 1; // Note: This won't match the auto-incremented id, but used for dummy
    d.unit_id = 10017; // Vargas ID
    d.unit_type_id = 1;
    d.element = "fire";
    d.unit_lv = 1;
    d.receive_date = 100;
    d.fe_bp = 100;
    d.fe_max_usable_bp = 200;
    d.base_hp = 1000;
    d.base_atk = 1000;
    d.base_def = 1000;
    d.base_heal = 1000;
    d.add_hp = 100;
    d.add_atk = 100;
    d.add_def = 100;
    d.add_heal = 100;
    d.ext_hp = 100;
    d.ext_atk = 100;
    d.ext_def = 100;
    d.ext_heal = 100;
    d.limit_over_hp = 200;
    d.limit_over_atk = 200;
    d.limit_over_def = 200;
    d.limit_over_heal = 200;
    d.exp = 2;
    d.total_exp = 5;

    resp.unit_info.emplace_back(d);

    UserPartyDeckInfo deck = {};
    deck.deck_type = 1;
    deck.user_unit_id = 1; // Now maps to id from user_units
    resp.party_deck_info.emplace_back(deck);

    resp.campaign_info.current_day = 1;
    resp.campaign_info.total_days = 96;
    resp.campaign_info.first_for_the_day = true;
    resp.campaign_info.id = 1;

    resp.summoner_journal.user_id = req.login_info.user_id; // we are really trusting the client here (bad)
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
