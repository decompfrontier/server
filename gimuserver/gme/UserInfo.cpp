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
    resp.team_info.reinforcement_deck.emplace_back(0);
    resp.team_info.reinforcement_deck.emplace_back(0);
    resp.team_info.reinforcement_deck.emplace_back(0);
    resp.team_info.user_id = resp.login_info.user_id;
    resp.team_info.level = 1;
    resp.team_info.exp = 0;
    resp.team_info.warehouse_count = 100;


    {
        UserUnitInfo d;
        d.user_id = resp.login_info.user_id;
        d.user_unit_id = 1;
        d.unit_type_id = 1;

        d.base_hp = 1000;
        d.add_hp = 1001;
        d.ext_hp = 1002;

        d.base_def = 1100;
        d.add_def = 1101;
        d.ext_def = 1102;

        d.base_heal = 1200;
        d.add_heal = 1201;
        d.ext_heal = 1202;

        d.base_atk = 1300;
        d.add_atk = 1301;
        d.ext_atk = 1302;

        d.limit_over_atk = 1400;
        d.limit_over_def = 1401;
        d.limit_over_heal = 1402;
        d.limit_over_hp = 1403;

        d.element = 1;
        d.unit_lv = 1;
        d.new_flag = 1;

        d.ext_count = 1500;
        d.receive_date = 100;
        d.fe_bp = 100;
        d.fe_used_bp = 0;
        d.fe_max_usable_bp = 200;
        d.unit_img_type = 0;


        d.exp = 1;
        d.total_exp = 1;

        d.unit_id = 50253;
        resp.unit_info.emplace_back(d);
    }

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
