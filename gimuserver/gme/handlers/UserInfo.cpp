#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/db/PacketInterface.hpp>
#include <gimuserver/gme/common/Common.hpp>

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
	// equip_info so they appear in the mission item bar.
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
	for (const auto& stack : warehouse)
	{
		if (isBattleConsumable(stack.item_id))
		{
			resp.equip_info.push_back(UserEquipItemInfo{
				.item_id = stack.item_id,
				.disp_order = equipSlot++,
				.item_num = stack.item_num,
			});
		}
	}
	resp.warehouse_info = std::move(warehouse);

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
