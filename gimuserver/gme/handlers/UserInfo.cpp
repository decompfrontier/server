#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/db/PacketInterface.hpp>
#include <gimuserver/gme/common/Common.hpp>

#include <ctime>

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

    const auto db = theDb();
	const auto identity = (co_await gme::getUserIdentity(db, req.login_info, true)).nonEmpty();

	// We must have a valid user entry in the database at this point.
	resp.login_info = std::move((co_await gme::getLoginInfo(db, identity)).nonEmpty());
	resp.team_info = std::move((co_await gme::getTeamInfo(db, identity)).nonEmpty());

	// UserInfo is the session-level refresh point for owned units. Clear any
	// persisted new flags before returning the full collection.
	//
	// The client keeps its own in-memory new-unit list and clears it during normal
	// gameplay, so the server only needs to reset the stored flags when the client
	// asks for a fresh UserInfo snapshot.
	co_await db::DatabaseInterface::update(
		db,
		"user_units",
		{
			db::Data("new", false),
			db::Lookup("user_id", identity.userId),
		});

    // We should always have at least one unit, since the game will not let users
    // delete their only unit on the squad.
	resp.unit_info = std::move((co_await db::PacketInterfaceFor<UserUnitInfo>::read(
		db,
		"user_units",
		{ db::Lookup("user_id", identity.userId) })).nonEmpty());

	auto partyDeckInfo = (co_await db::PacketInterfaceFor<UserPartyDeckInfo>::read(
		db,
		"user_decks",
		{ db::Lookup("user_id", identity.userId) })).nonEmpty();
	resp.party_deck_info = std::move(partyDeckInfo);

	resp.unit_dictionary = std::move((co_await db::PacketInterfaceFor<UserUnitDictionary>::read(
		db,
		"user_unit_dictionary",
		{ db::Lookup("user_id", identity.userId) })).data);

	// Owned items now come from the user_items table (seeded by the tutorial,
	// grown by mission drops) instead of the old hardcoded potion.  The full
	// inventory populates warehouse_info; battle-consumable items (ItemMst
	// item_type == 1, e.g. the tutorial healing potion) also populate
	// equip_info so they appear in the mission item bar.  Stacks at 0 keep
	// their row (ItemSell / ItemSphereEqp decrement without deleting so
	// instance ids stay stable) but are filtered off the wire; every species
	// ever stacked still feeds the item dictionary, and favorited stacks are
	// reported through item_favorite (VSRPkdId) so locks survive a reload.
	auto warehouse = (co_await db::PacketInterfaceFor<UserWarehouseInfo>::read(
		db,
		"user_items",
		{ db::Lookup("user_id", identity.userId) })).data;

	const auto& itemMst = theServer()->cache().itemMst();
	const auto isBattleConsumable = [&itemMst](uint32_t itemId) {
		for (const auto& m : itemMst)
		{
			if (m.id == itemId)
			{
				return m.item_type == 1;
			}
		}
		return false;
	};

	uint32_t equipSlot = 0;
	std::vector<UserWarehouseInfo> visibleStacks;
	visibleStacks.reserve(warehouse.size());
	for (auto& stack : warehouse)
	{
		resp.item_dictionary_info.push_back(UserItemDictionaryInfo{
			.item_id = stack.item_id,
		});

		if (stack.item_num == 0)
			continue;

		if (stack.favorite_flg != 0)
		{
			// "::" — the packet struct, not GmeHandlers::ItemFavorite (the
			// handler declared in Handlers.hpp shadows it in this scope).
			resp.item_favorite.push_back(::ItemFavorite{
				.instance_id = stack.instance_id,
				.favorite = 1,
			});
		}

		if (isBattleConsumable(stack.item_id))
		{
			resp.equip_info.push_back(UserEquipItemInfo{
				.item_id = stack.item_id,
				.disp_order = equipSlot++,
				.item_num = stack.item_num,
			});
		}

		visibleStacks.push_back(std::move(stack));
	}
	resp.warehouse_info = std::move(visibleStacks);

	// Cleared-mission history (UT1SVg59) — THE progression driver.  The client
	// evaluates feature unlocks against this list: F_FUNCTION_RELEASE_MST rows
	// (condition type 2 = "mission <param> cleared") gate functions 8-19, and
	// the hardcoded early-feature gates (town etc.) key off the same set plus
	// tutorial_status.  Backed by user_campaign_missions state=2 rows, written
	// by MissionEnd and CampaignBattleEnd.
	{
		const auto rows = co_await db->execSqlCoro(
			"SELECT mission_id, clear_count, last_cleared_at"
			" FROM user_campaign_missions WHERE user_id = $1 AND state = 2;",
			identity.userId);
		for (const auto& row : rows)
		{
			::UserClearMissionInfo cleared = {};
			cleared.user_id = identity.userId;
			try
			{
				cleared.mission_id = std::stoi(row["mission_id"].as<std::string>());
			}
			catch (...)
			{
				continue;
			}
			cleared.clear_cnt = row["clear_count"].as<int32_t>();
			if (const auto epoch = row["last_cleared_at"].as<int64_t>(); epoch > 0)
			{
				// setClearDate is a string setter; "YYYY-MM-DD hh:mm:ss" until a
				// capture proves otherwise (KDL doc marks it UNVERIFIED).
				std::tm tmv = {};
				const time_t t = static_cast<time_t>(epoch);
				localtime_s(&tmv, &t);
				char buf[24] = {};
				std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tmv);
				cleared.clear_date = buf;
			}
			resp.clear_mission_info.push_back(std::move(cleared));
		}
	}

	// Favorited/locked units (3kcmQy7B) — UnitFavorite persists the flag;
	// reporting it back makes locks survive a reload.
	{
		const auto rows = co_await db->execSqlCoro(
			"SELECT user_unit_id FROM user_units"
			" WHERE user_id = $1 AND favorite_flg = 1;",
			identity.userId);
		for (const auto& row : rows)
		{
			resp.favorite.push_back(::UserFavorite{
				.user_unit_id = row["user_unit_id"].as<int32_t>(),
				.favorite = 1,
				.unit_img_type = 0,
			});
		}
	}

	// Town state — the three arrays travel together (§6.8): every location in
	// town_location_info needs a matching town_location_detail entry or the
	// town scene loader null-derefs.  Rows are provisioned by the town unlock
	// (CLI `unlocktown` for now); a fresh account has none and sends empty
	// arrays, which the client treats as town-not-available.
	{
		const auto facilityRows = co_await db->execSqlCoro(
			"SELECT facility_id, lv, karma FROM user_town_facilities WHERE user_id = $1;",
			identity.userId);
		for (const auto& row : facilityRows)
		{
			resp.town_facility_info.push_back(UserTownFacilityInfo{
				.user_id = identity.userId,
				.facility_id = row["facility_id"].as<int32_t>(),
				.lv = row["lv"].as<int32_t>(),
				.karma = row["karma"].as<int32_t>(),
			});
		}

		const auto locationRows = co_await db->execSqlCoro(
			"SELECT location_id, lv, karma FROM user_town_locations WHERE user_id = $1;",
			identity.userId);
		for (const auto& row : locationRows)
		{
			const auto locationId = row["location_id"].as<int32_t>();
			resp.town_location_info.push_back(UserTownLocationInfo{
				.user_id = identity.userId,
				.location_id = locationId,
				.lv = row["lv"].as<int32_t>(),
				.karma = row["karma"].as<int32_t>(),
			});
			resp.town_location_detail.push_back(UserTownLocationDetail{
				.user_id = identity.userId,
				.unk = locationId,
				.unk2 = {},  // epoch — tap period not yet started
				.unk3 = 0,   // no taps accumulated
				.unk4 = "",
			});
		}
	}

    resp.campaign_info.current_day = 1;
    resp.campaign_info.total_days = 96;
    resp.campaign_info.first_for_the_day = false;
    resp.campaign_info.id = 1;

    resp.summoner_journal.user_id = identity.userId;
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
