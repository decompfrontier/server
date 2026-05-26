#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/archive/UnitArchiver.hpp>
#include <gimuserver/db/UserUnitService.hpp>

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
    resp.login_info.tutorial_status = 0;
    resp.login_info.feature_gate = 0;

    UserTeamInfo team = {};
    resp.team_info.reinforcement_deck.emplace_back(0);
    resp.team_info.reinforcement_deck.emplace_back(0);
    resp.team_info.reinforcement_deck.emplace_back(0);
    resp.team_info.user_id = resp.login_info.user_id;
    resp.team_info.level = 1;
    resp.team_info.exp = 0;
    resp.team_info.warehouse_count = 100;
    resp.team_info.add_unit_count = 100;
    resp.team_info.max_unit_count = 100;

    const auto db = theDb();
    if (!co_await UserUnitService::getUnits(db, resp.login_info.user_id, resp.unit_info))
    {
        co_return HandleResult::error("Database error", "Unable to load user units");
    }

    if (resp.unit_info.empty())
    {
        UserUnitInfo d = {
            .unit_id = 50253,
            .unit_type_id = 1,
        };

        if (!UnitArchiver::Instance().Lookup(d))
        {
            co_return HandleResult::error("Archive error", "Unable to find placeholder unit record");
        }

        d.user_id = resp.login_info.user_id;

        if (!co_await UserUnitService::AddUnit(db, resp.login_info.user_id, d))
        {
            co_return HandleResult::error("Database error", "Unable to add placeholder user unit");
        }

        if (!co_await UserUnitService::getUnits(db, resp.login_info.user_id, resp.unit_info) || resp.unit_info.empty())
        {
            co_return HandleResult::error("Database error", "Unable to reload user units");
        }
    }

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
