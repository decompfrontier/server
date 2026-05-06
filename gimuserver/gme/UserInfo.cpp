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

	const auto& infoRows = co_await theDb()->execSqlCoro(
		"SELECT username, level, exp, zel, karma, brave_coin,"
		" free_gems, paid_gems, energy,"
		" max_unit_count, max_warehouse_count,"
		" summon_tickets, rainbow_coins, colosseum_tickets,"
		" total_brave_points, avail_brave_points, active_deck, want_gift"
		" FROM userinfo WHERE id=$1;",
		std::string("0839899613932562")
	);
	if (infoRows.empty())
	{
		LOG_ERROR << "UserInfo: no userinfo row for '0839899613932562'";
		co_return HandleResult::success("{}");
	}
	const auto& infoRow = infoRows.at(0);
	const int32_t level = infoRow["level"].as<int32_t>();

	// Level-gated caps from the already-cached user_level.json MST.
	const auto& prog = theServer()->cache().initializeResp().progression;
	const UserLevelMst* lv = nullptr;
	for (const auto& e : prog) { if (e.level == level) { lv = &e; break; } }
	const int32_t maxAP      = lv ? lv->action_points    : 100;
	const int32_t deckCost   = lv ? lv->deck_cost        : 20;
	const int32_t friendBase = lv ? lv->friend_count     : 50;
	const int32_t friendAdd  = lv ? lv->add_friend_count : 0;

	resp.login_info.user_id           = "0839899613932562"; // packet-gen UUID
	resp.login_info.account_id        = "0839899613932562";
	resp.login_info.handle_name       = infoRow["username"].as<std::string>();
	resp.login_info.tutorial_end_flag = true;
	resp.login_info.tutorial_status   = 0;
	resp.login_info.feature_gate      = 0;

	auto& ti = resp.team_info;
	ti.user_id             = resp.login_info.user_id;
	ti.level               = level;
	ti.exp                 = infoRow["exp"].as<int64_t>();
	ti.zel                 = infoRow["zel"].as<int64_t>();
	ti.karma               = infoRow["karma"].as<int64_t>();
	ti.brave_coin          = infoRow["brave_coin"].as<int32_t>();
	ti.action_point        = infoRow["energy"].as<int32_t>();
	ti.max_action_point    = maxAP;
	ti.deck_cost           = deckCost;
	ti.max_friend_count    = friendBase;
	ti.add_friend_count    = friendAdd;
	ti.max_unit_count      = infoRow["max_unit_count"].as<int32_t>();
	ti.add_unit_count      = 0;
	ti.warehouse_count     = infoRow["max_warehouse_count"].as<int32_t>();
	ti.add_warehouse_count = 0;
	ti.active_deck         = infoRow["active_deck"].as<int32_t>();
	ti.summon_ticket       = infoRow["summon_tickets"].as<int32_t>();
	ti.rainbow_coin        = infoRow["rainbow_coins"].as<int32_t>();
	ti.colosseum_ticket    = infoRow["colosseum_tickets"].as<int32_t>();
	ti.brave_points_total   = infoRow["total_brave_points"].as<int32_t>();
	ti.current_brave_points = infoRow["avail_brave_points"].as<int32_t>();
	ti.want_gift           = infoRow["want_gift"].as<std::string>();
	ti.paid_gems           = infoRow["paid_gems"].as<int32_t>();
	ti.free_gems           = infoRow["free_gems"].as<int32_t>();
	ti.reinforcement_deck.emplace_back(0);
	ti.reinforcement_deck.emplace_back(0);
	ti.reinforcement_deck.emplace_back(0);

    // Load units from DB (seeded by GimuServer::SeedDefaultUnits on boot)
    const auto& unitRows = co_await theDb()->execSqlCoro(
        "SELECT id, unit_id, unit_lv,"
        " base_hp,  add_hp,  ext_hp,  limit_over_hp,"
        " base_atk, add_atk, ext_atk, limit_over_atk,"
        " base_def, add_def, ext_def, limit_over_def,"
        " base_heal,add_heal,ext_heal,limit_over_heal,"
        " exp, total_exp,"
        " skill_id, skill_lv, extra_skill_id, extra_skill_lv, leader_skill_id,"
        " element, fe_bp, fe_max_usable_bp, unit_type_id,"
        " eqip_item_id, eqip_item_frame_id, eqip_item_id2, eqip_item_frame_id2"
        " FROM user_units WHERE user_id=$1;",
        resp.login_info.user_id
    );

    for (const auto& row : unitRows)
    {
        UserUnitInfo d = {};
        d.user_id          = resp.login_info.user_id;
        d.user_unit_id     = row["id"].as<int32_t>();
        d.unit_id          = std::stoi(row["unit_id"].as<std::string>());
        d.unit_type_id     = row["unit_type_id"].as<int32_t>();
        d.unit_lv          = row["unit_lv"].as<int32_t>();
        d.exp              = row["exp"].as<int32_t>();
        d.total_exp        = row["total_exp"].as<int32_t>();
        d.base_hp          = row["base_hp"].as<int32_t>();
        d.add_hp           = row["add_hp"].as<int32_t>();
        d.ext_hp           = row["ext_hp"].as<int32_t>();
        d.limit_over_hp    = row["limit_over_hp"].as<int32_t>();
        d.base_atk         = row["base_atk"].as<int32_t>();
        d.add_atk          = row["add_atk"].as<int32_t>();
        d.ext_atk          = row["ext_atk"].as<int32_t>();
        d.limit_over_atk   = row["limit_over_atk"].as<int32_t>();
        d.base_def         = row["base_def"].as<int32_t>();
        d.add_def          = row["add_def"].as<int32_t>();
        d.ext_def          = row["ext_def"].as<int32_t>();
        d.limit_over_def   = row["limit_over_def"].as<int32_t>();
        d.base_heal        = row["base_heal"].as<int32_t>();
        d.add_heal         = row["add_heal"].as<int32_t>();
        d.ext_heal         = row["ext_heal"].as<int32_t>();
        d.limit_over_heal  = row["limit_over_heal"].as<int32_t>();
        d.element          = row["element"].as<std::string>();
        d.leader_skill_id  = row["leader_skill_id"].as<int32_t>();
        d.skill_id         = row["skill_id"].as<int32_t>();
        d.skill_lv         = row["skill_lv"].as<int32_t>();
        d.extra_skill_id   = row["extra_skill_id"].as<int32_t>();
        d.extra_skill_lv   = row["extra_skill_lv"].as<int32_t>();
        d.equipitem_id     = row["eqip_item_id"].as<int32_t>();
        d.equipitem_frame_id  = row["eqip_item_frame_id"].as<int32_t>();
        d.equipitem_id2    = row["eqip_item_id2"].as<int32_t>();
        d.equipitem_frame_id2 = row["eqip_item_frame_id2"].as<int32_t>();
        d.fe_bp            = row["fe_bp"].as<int32_t>();
        d.fe_used_bp       = 0;
        d.fe_max_usable_bp = row["fe_max_usable_bp"].as<int32_t>();
        d.new_flag         = true;
        d.unit_img_type    = 0;
        d.omni_level       = 0;
        d.ext_count        = 0;
        d.unk              = 0;
        d.unk2             = 0;
        d.receive_date     = {}; // epoch (chrono::time_point)
        d.extra_passive_skill_id  = 0;
        d.extra_passive_skill_id2 = 0;
        d.add_extra_passive_skill_id = 0;
        d.fe_skill_info    = "";
        resp.unit_info.emplace_back(d);
    }

    // Party decks — load persisted deck from DB (written by DeckEdit).  Fall
    // back to seeding all 10 slots with the first owned unit when no rows
    // exist yet (i.e. before the player edits any deck for the first time).
    {
        const auto& deckRows = co_await theDb()->execSqlCoro(
            "SELECT deck_type, deck_num, user_unit_id, member_type, disp_order"
            " FROM user_party_decks WHERE user_id=$1"
            " ORDER BY deck_type, deck_num, disp_order;",
            resp.login_info.user_id
        );

        if (!deckRows.empty())
        {
            for (const auto& row : deckRows)
            {
                UserPartyDeckInfo d = {};
                d.deck_type    = row["deck_type"].as<int32_t>();
                d.deck_num     = row["deck_num"].as<int32_t>();
                d.user_unit_id = row["user_unit_id"].as<int32_t>();
                d.member_type  = row["member_type"].as<int32_t>();
                d.disp_order   = row["disp_order"].as<int32_t>();
                resp.party_deck_info.emplace_back(d);
            }
        }
        else
        {
            // No saved decks yet — generate a default (one entry per slot, first
            // owned unit as leader).  The player can persist their own composition
            // via DeckEdit at any time.
            const int32_t firstUnitId = resp.unit_info.empty() ? 0 : resp.unit_info.front().user_unit_id;
            for (int i = 0; i < 10; i++) {
                UserPartyDeckInfo deck = {};
                deck.deck_num     = i;
                deck.deck_type    = 1;
                deck.user_unit_id = firstUnitId;
                resp.party_deck_info.emplace_back(deck);
            }
        }
    }

    // Town facilities
    const auto& facilityRows = co_await theDb()->execSqlCoro(
        "SELECT facility_id, lv, karma FROM user_town_facilities WHERE user_id=$1;",
        resp.login_info.user_id
    );
    for (const auto& row : facilityRows)
    {
        UserTownFacilityInfo f = {};
        f.user_id     = resp.login_info.user_id;
        f.facility_id = row["facility_id"].as<int32_t>();
        f.lv          = row["lv"].as<int32_t>();
        f.karma       = row["karma"].as<int32_t>();
        resp.town_facility_info.emplace_back(f);
    }

    // Town locations — populate both info and a matching detail entry for each.
    // The client dereferences the detail for every location in the info array;
    // sending info without a corresponding detail causes a null-pointer crash
    // in the town scene loader.
    const auto& locationRows = co_await theDb()->execSqlCoro(
        "SELECT location_id, lv, karma FROM user_town_locations WHERE user_id=$1;",
        resp.login_info.user_id
    );
    for (const auto& row : locationRows)
    {
        UserTownLocationInfo l = {};
        l.user_id     = resp.login_info.user_id;
        l.location_id = row["location_id"].as<int32_t>();
        l.lv          = row["lv"].as<int32_t>();
        l.karma       = row["karma"].as<int32_t>();
        resp.town_location_info.emplace_back(l);

        UserTownLocationDetail d = {};
        d.user_id = resp.login_info.user_id;
        d.unk     = l.location_id;
        d.unk2    = {};  // epoch — tap period not yet started
        d.unk3    = 0;   // no taps accumulated
        d.unk4    = "";
        resp.town_location_detail.emplace_back(d);
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

    // Inject PermitPlace unlock data.  The generated PermitPlace struct is a
    // stub ("INVALID" key), so we replace the serialised empty array in-place.
    // Each entry unlocks one entity type — the client reads exactly one key per
    // entry to determine which area/land/gate/mission/dungeon is accessible.
    //   VjCY7rX4 = area, 9C64Qwe0 = land, 0Cq2AlXW = gate,
    //   j28VNcUW = mission, MHx05sXt = dungeon
    //
    // Build a comprehensive allow-list once and reuse it for every UserInfo call.
    // Ranges are derived from the F_*_MST entry counts in version_info.json:
    //   F_AREA_MST    669 entries, max id ~ 411  → use 1-1000
    //   F_LAND_MST    147 entries, ~26 unique ids → use 1-200
    //   F_GATE_MST     95 entries, ~5  unique ids → use 1-100
    //   F_MISSION_MST 1118 entries, 3433 count   → use 1-4000
    //   F_DUNGEON_MST 1002 entries, 1532 count   → use 1-2000
    // The client silently ignores entries for IDs that don't exist in its local MST.
    static constexpr std::string_view kEmptyPermit = R"("yXNM8kL3":[])";
    static const std::string kFullPermit = []() {
        std::string s;
        s.reserve(200'000);
        s += R"("yXNM8kL3":[)";
        bool first = true;
        auto add = [&](std::string_view key, int id) {
            if (!first) s += ',';
            s += "{\"";
            s += key;
            s += "\":\"";
            s += std::to_string(id);
            s += "\"}";
            first = false;
        };
        for (int i = 1; i <= 1000; ++i) add("VjCY7rX4", i); // areas
        for (int i = 1; i <=  200; ++i) add("9C64Qwe0", i); // lands
        for (int i = 1; i <=  100; ++i) add("0Cq2AlXW", i); // gates
        for (int i = 1; i <= 4000; ++i) add("j28VNcUW", i); // missions
        for (int i = 1; i <= 2000; ++i) add("MHx05sXt", i); // dungeons
        s += ']';
        return s;
    }();

    const auto pos = buffer.find(kEmptyPermit);
    if (pos != std::string::npos)
        buffer.replace(pos, kEmptyPermit.size(), kFullPermit);
    else
        LOG_WARN << "UserInfo: yXNM8kL3 token not found in serialised buffer — PermitPlace not injected";

    co_return HandleResult::success(buffer);
}
