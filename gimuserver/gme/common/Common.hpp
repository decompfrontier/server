#pragma once

#include <gimuserver/archive/UnitArchiver.hpp>
#include <gimuserver/db/PacketInterface.hpp>
#include <gimuserver/gme/common/Energy.hpp>

#include <drogon/orm/DbClient.h>

#include <cstdint>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace gme
{

/*!
* Local user identity resolved from a Gumi Live login.
*/
struct UserIdentity
{
	std::string gumiUserId;
	std::string userId;
};

/*!
* Builds a user-unit packet from curated unit archive data.
*
* @param unit_id Unit id to look up in the archive.
* @param unit_type_id Unit type to use when populating stats.
* @return Populated packet, or std::nullopt when archive data is missing or
* unsupported.
*/
inline std::optional<UserUnitInfo> fromArchivedUnit(uint32_t unit_id, uint32_t unit_type_id)
{
	const auto unitRecord = UnitArchiver::instance().lookup(unit_id);
	if (!unitRecord)
	{
		return std::nullopt;
	}

	UserUnitInfo unit{};
	if (!UnitArchiver::populatePacket(*unitRecord, unit_type_id, unit))
	{
		return std::nullopt;
	}

	return unit;
}

/*!
* Adds a user-owned unit row and updates the packet with database-owned fields.
*
* @param database Database client or transaction to use.
* @param identity Resolved user identity that owns the unit.
* @param unit Unit packet to persist.
* @param isNew Whether the inserted unit should be marked new for the client.
* @return Persisted unit packet populated with database-owned fields.
*/
inline drogon::Task<db::InterfaceResult<UserUnitInfo>> addUserUnit(
	const db::Database database,
	const UserIdentity identity,
	const UserUnitInfo unit,
	const bool isNew = true)
{
	auto packet = unit;
	packet.user_id = identity.userId;
	packet.is_new = isNew;
	const auto result = co_await db::PacketInterfaceFor<UserUnitInfo>::insert(
		database,
		"user_units",
		packet);

	packet.user_unit_id = result.front<uint32_t>("user_unit_id");
	packet.received_order = packet.user_unit_id;

	co_await db::PacketInterfaceFor<UserUnitDictionary>::insert(
		database,
		"user_unit_dictionary",
		UserUnitDictionary{
			.user_id = identity.userId,
			.unit_id = packet.unit_id,
		});

	co_return db::InterfaceResult<UserUnitInfo>{
		.data = std::move(packet),
		.affected = result.affected,
	};
}

/*!
* Credits an item stack to the owning user's warehouse.
*
* Stacks are keyed by (user_id, item_id): a repeat drop increments item_num
* rather than creating a second row.  Returns the resulting quantity.
*
* @param database Database client or transaction to use.
* @param identity Resolved user identity that owns the item.
* @param itemId Item master id to credit.
* @param quantity Amount to add (default 1).
* @return Number of affected rows.
*/
inline drogon::Task<db::InterfaceResult<>> addUserItem(
	const db::Database database,
	const UserIdentity identity,
	const uint32_t itemId,
	const uint32_t quantity = 1)
{
	if (!database || identity.userId.empty() || itemId == 0)
	{
		LOG_ERROR << "Invalid addUserItem call: "
			<< "db=" << static_cast<bool>(database)
			<< ", user_id_empty=" << identity.userId.empty()
			<< ", item_id=" << itemId;
		throw std::invalid_argument("Invalid addUserItem call");
	}

	// Bump the stack if it exists, else insert a new one.  item_num accumulates
	// rather than being replaced, so repeated grants stack.
	co_return co_await db::DatabaseInterface::upsert(
		database,
		"user_items",
		{
			db::Data("user_id", identity.userId),
			db::Data("item_id", itemId),
			db::Data("item_num", quantity),
		},
		{ "user_id", "item_id" },
		{ "item_num" });
}

/*!
* Returns any spheres equipped on soon-to-be-consumed units to the owner's
* warehouse.
*
* UnitSell / UnitMix / UnitEvo delete user_units rows (sold units, fusion
* fodder, evo materials).  Spheres equipped on those units are owned items —
* deleting the row without this call would destroy them silently.  Call BEFORE
* the DELETE, with the same pre-validated integer id list its IN clause uses.
*
* @param database Database client or transaction to use.
* @param identity Resolved user identity that owns the units.
* @param userUnitIdList Comma-joined user_unit_id list (validated integers).
*/
inline drogon::Task<void> returnEquippedSpheres(
	const db::Database database,
	const UserIdentity identity,
	const std::string& userUnitIdList)
{
	if (!database || identity.userId.empty() || userUnitIdList.empty())
		co_return;

	// Callers hand us the same comma-joined id string their DELETE uses, so
	// split it back into bound values rather than splicing it into SQL.  When
	// those DELETEs move onto the typed interface this should take the id
	// vector directly and the round trip disappears.
	db::Values userUnitIds;
	for (size_t start = 0; start <= userUnitIdList.size();)
	{
		const auto end = userUnitIdList.find(',', start);
		const auto token = userUnitIdList.substr(
			start, end == std::string::npos ? std::string::npos : end - start);
		if (!token.empty())
		{
			userUnitIds.emplace_back(
				static_cast<uint64_t>(std::stoull(token)));
		}

		if (end == std::string::npos)
		{
			break;
		}

		start = end + 1;
	}

	if (userUnitIds.empty())
		co_return;

	const auto result = co_await db::DatabaseInterface::read(
		database,
		"user_units",
		{
			db::Data("eqip_item_id"),
			db::Data("eqip_item_id2"),
			db::Lookup("user_id", identity.userId),
			db::LookupIn("user_unit_id", userUnitIds),
		});
	for (const auto& row : result.data)
	{
		for (const auto col : { "eqip_item_id", "eqip_item_id2" })
		{
			const auto itemId = row[col].as<uint32_t>();
			if (itemId != 0)
			{
				co_await addUserItem(database, identity, itemId, 1);
			}
		}
	}
}

/*!
* Looks up player progression MST data for a specific user level.
*
* The progression cache is expected to be ordered by level, with level 1 at
* index 0. This helper centralizes that assumption and validates the row before
* returning it.
*
* @param level Player level to look up.
* @return Progression row for the level, or std::nullopt if unavailable.
*/
inline std::optional<UserLevelMst> getLevelMst(uint32_t level)
{
	const auto& progression = theServer()->cache().initializeResp().progression;
	if (level == 0 || level > progression.size())
	{
		LOG_ERROR << "Unable to find user level MST for level " << level;
		return std::nullopt;
	}

	const auto& mst = progression[level - 1];
	if (mst.level != level)
	{
		LOG_ERROR << "User level progression is not ordered at level " << level;
		return std::nullopt;
	}

	return mst;
}

/*!
* Seeds the default normal decks when the owning user has no deck rows yet.
* The starter unit is placed in the middle party position for decks 0
* through 9. Existing deck rows are ignored and return zero affected rows.
*
* @param database Database client or transaction to use.
* @param identity Resolved user identity that owns the deck rows.
* @param starter_unit Starter unit to place in the default decks.
* @return Number of affected rows.
*/
inline drogon::Task<db::InterfaceResult<>> addDefaultDecks(
	const db::Database database,
	const UserIdentity identity,
	const UserUnitInfo starter_unit)
{
	if (!database || identity.userId.empty() || starter_unit.user_unit_id == 0)
	{
		LOG_ERROR << "Invalid addDefaultDecks call: "
			<< "db=" << static_cast<bool>(database)
			<< ", user_id_empty=" << identity.userId.empty()
			<< ", user_unit_id=" << starter_unit.user_unit_id;
		throw std::invalid_argument("Invalid addDefaultDecks call");
	}

	auto result = co_await database->execSqlCoro(
		"WITH RECURSIVE deck_nums(deck_num) AS ("
		"SELECT 0 "
		"UNION ALL "
		"SELECT deck_num + 1 FROM deck_nums WHERE deck_num < 9"
		") "
		"INSERT INTO user_decks ("
		"user_id, "
		"user_unit_id, "
		"deck_type, "
		"deck_num, "
		"member_type, "
		"disp_order"
		") "
		"SELECT "
		"$1, "
		"$2, "
		"1, "
		"deck_nums.deck_num, "
		// The client treats member_type 0 as the party leader.
		"0, "
		"2 "
		"FROM deck_nums "
		"WHERE NOT EXISTS ("
		"SELECT 1 "
		"FROM user_decks "
		"WHERE user_decks.user_id = $1"
		");",
		identity.userId,
		starter_unit.user_unit_id);

	co_return db::InterfaceResult<>{
		.data = {},
		.affected = result.affectedRows(),
	};
}

/*!
* Persists deck rows posted by DeckEditRequest.
* For each deck included in the request, existing rows are replaced with the
* occupied slots sent by the client. Missing slots are treated as removed.
*
* @param database Database client or transaction to use.
* @param identity Resolved user identity that owns the deck rows.
* @param decks Deck slot packets to persist.
* @return Number of affected rows.
*/
inline drogon::Task<db::InterfaceResult<>> updateDecks(
	const db::Database database,
	const UserIdentity identity,
	const std::vector<UserPartyDeckInfo> decks)
{
	if (!database || identity.userId.empty())
	{
		LOG_ERROR << "Invalid updateDecks call: "
			<< "db=" << static_cast<bool>(database)
			<< ", user_id_empty=" << identity.userId.empty();
		throw std::invalid_argument("Invalid updateDecks call");
	}

	// For each modified party, the client sends the full current party state.
	// Any missing slot should therefore be treated as a removed unit.
	//
	// The simplest way to persist that is to clear each affected deck first,
	// then insert the UserPartyDeckInfo rows the client sent.
	std::set<std::pair<int32_t, int32_t>> affectedDecks;
	for (const auto& deck : decks)
	{
		affectedDecks.emplace(deck.deck_type, deck.deck_num);
	}

	size_t affected = 0;
	for (const auto& [deckType, deckNum] : affectedDecks)
	{
		const auto result = co_await db::DatabaseInterface::remove(
			database,
			"user_decks",
			{
				db::Lookup("user_id", identity.userId),
				db::Lookup("deck_type", deckType),
				db::Lookup("deck_num", deckNum),
			});
	}

	for (const auto& deck : decks)
	{
		const auto insert = co_await db::PacketInterfaceFor<UserPartyDeckInfo>::insert(
			database,
			"user_decks",
			deck,
			{ db::Data("user_id", identity.userId) });
		affected += insert.affected;
	}

	co_return db::InterfaceResult<>{
		.data = {},
		.affected = affected,
	};
}

/*!
* Builds a login-info packet from the persisted user row.
*
* @param database Database client or transaction to use.
* @param identity Resolved user identity to read.
* @return Login info packet populated from the database.
*/
inline drogon::Task<db::InterfaceResult<LoginInfoResp>> getLoginInfo(
	const db::Database database,
	const UserIdentity identity)
{
	auto result = co_await db::PacketInterfaceFor<LoginInfoResp>::read(
		database,
		"user_info",
		{
			db::Lookup("gumi_user_id", identity.gumiUserId),
			db::Lookup("id", identity.userId),
		});
	auto packet = std::move(result.nonEmpty().front());

	// The database stores tutorial_status. Before a login-info packet is sent back,
	// handlers keep tutorial_end_flag consistent with that status. In the current
	// server model, status 12 or greater means the tutorial is complete.
	packet.tutorial_end_flag = packet.tutorial_status >= 12;

	co_return db::InterfaceResult<LoginInfoResp>{
		.data = std::move(packet),
		.affected = result.affected,
	};
}

/*!
* Builds a team-info packet from the persisted user row.
*
* @param database Database client or transaction to use.
* @param identity Resolved user identity to read.
* @return Team info packet populated from the database and derived cache data.
*/
inline drogon::Task<db::InterfaceResult<UserTeamInfo>> getTeamInfo(
	const db::Database database,
	const UserIdentity identity)
{
	auto result = co_await db::PacketInterfaceFor<UserTeamInfo>::read(
		database,
		"user_info",
		{ db::Lookup("id", identity.userId) });
	auto packet = std::move(result.nonEmpty().front());

	packet.reinforcement_deck.emplace_back(0);
	packet.reinforcement_deck.emplace_back(0);
	packet.reinforcement_deck.emplace_back(0);
	packet.add_unit_count = 100;

	if (const auto mst = getLevelMst(packet.level))
	{
		packet.deck_cost = mst->deck_cost;
		packet.max_action_point = mst->energy;
		packet.max_friend_count = mst->friend_count;
		packet.add_friend_count = mst->add_friend_count;
	}

	// Calculate the current energy points of the user.
	const auto energyFullTs = (co_await db::DatabaseInterface::read(
		database,
		"user_info",
		{
			db::Data("energy_full_ts"),
			db::Lookup("id", identity.userId),
		})).front<uint64_t>("energy_full_ts");
	packet.energy_full_seconds = UserEnergy::derive(
		packet.level,
		energyFullTs,
		packet.energy);

	co_return db::InterfaceResult<UserTeamInfo>{
		.data = std::move(packet),
		.affected = result.affected,
	};
}

/*!
* Resolves and validates the current local user identity.
*
* The server stores one current Gumi Live user id, then maps it to the local
* user_info id. Requests must send the same Gumi Live id, and normally must also
* send the same user id. During user creation, the client may not have a local
* user id yet, so allowUnspecifiedUser lets callers accept an empty request
* user_id and use the database value instead.
*
* @param database Database client or transaction to use.
* @param req Login info from the request packet.
* @param allowUnspecifiedUser True when an empty request user_id is allowed.
* @return Resolved Gumi Live id and local user id.
*/
inline drogon::Task<db::InterfaceResult<UserIdentity>> getUserIdentity(
	const db::Database database,
	const LoginInfoReq req,
	const bool allowUnspecifiedUser=false)
{
	const auto gumiUserId = (co_await db::DatabaseInterface::read(
		database,
		"gumi_live_users",
		{ db::Data("id") })).front<std::string>("id");
	if (gumiUserId != req.gumi_live_userid)
	{
		LOG_ERROR << "Gumi Live user ID mismatch: client sent " << req.gumi_live_userid
			<< ", database has " << gumiUserId;
		throw std::runtime_error("Gumi Live user ID mismatch: client sent "
			+ req.gumi_live_userid + ", database has " + gumiUserId);
	}

	// Upon user creation, the client might not have a user Id stored locally yet,
	// so we allow the client to omit it in the request, and instead fallback to
	// whatever user ID we have in the database.
	if (!allowUnspecifiedUser && req.user_id.empty())
	{
		LOG_ERROR << "Missing user_id; request did not include user_id";
		throw std::runtime_error("Missing user_id; request did not include user_id");
	}

	auto user = co_await db::DatabaseInterface::read(
		database,
		"user_info",
		{
			db::Data("id"),
			db::Lookup("gumi_user_id", gumiUserId),
	});
	std::string userId;
	if (user.affected > 0)
	{
		userId = user.front<std::string>("id");
	}

	// If the client gave us a user_id, we require it to match the database.
	if (!req.user_id.empty() && userId != req.user_id)
	{
		LOG_ERROR << "User ID mismatch for Gumi Live user " << gumiUserId
			<< ": client sent " << req.user_id
			<< ", database has " << userId;
		throw std::runtime_error("User ID mismatch for Gumi Live user "
			+ gumiUserId + ": client sent " + req.user_id + ", database has " + userId);
	}

	co_return db::InterfaceResult<UserIdentity>{
		.data = {
			.gumiUserId = gumiUserId,
			.userId = userId,
		},
		// This means we must have found a user for the nonEmpty() check to
		// succeed.
		.affected = user.affected,
	};
}


}
