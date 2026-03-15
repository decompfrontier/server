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
    resp.login_info.user_id = "0839899613932562"; // I think this is a random UUID according to packet-gen

    // TEMP HACK!! Skip tutorial flag and put a real name
    resp.login_info.account_id = "12345678";
    resp.login_info.handle_name = "OfflineMod!";
    resp.login_info.tutorial_end_flag = true;
    resp.login_info.tutorial_status = 12;
    resp.login_info.feature_gate = 0;

    UserTeamInfo team = {};
    resp.team_info.brave_coin = 4200;
    resp.team_info.action_point = 431;
    resp.team_info.add_friend_count = 50;
    resp.team_info.max_friend_count = 50;
    resp.team_info.warehouse_count = 5500;
    resp.team_info.free_gems = 10000;
    resp.team_info.summon_ticket = 99;
    resp.team_info.level = 999;
    resp.team_info.karma = 99000000;
    resp.team_info.zel = 99000000;
    resp.team_info.deck_cost = 843;
    resp.team_info.reinforcement_deck.emplace_back(0);
    resp.team_info.reinforcement_deck.emplace_back(0);
    resp.team_info.reinforcement_deck.emplace_back(0);
    resp.team_info.max_action_point = 431;
    resp.team_info.paid_gems = 20000;
    resp.team_info.user_id = resp.login_info.user_id;
    resp.team_info.max_unit_count = 4000;

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

    for (int i = 0; i < 10; i++) {
        UserPartyDeckInfo deck = {};
        deck.deck_num = i;
        deck.deck_type = 1;
        deck.user_unit_id = 1; // Now maps to id from user_units
        resp.party_deck_info.emplace_back(deck);
    }

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
