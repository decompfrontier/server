#pragma once

#include "Handlers.hpp"

#include <gimuserver/archive/UnitArchiver.hpp>
#include <gimuserver/db/PacketInterface.hpp>

#include <drogon/orm/DbClient.h>

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
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
* Existing slots are updated first; missing slots are inserted.
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

	size_t affected = 0;
	for (const auto& deck : decks)
	{
		// Try to update an existing row first.
		const auto write = co_await db::PacketInterfaceFor<UserPartyDeckInfo>::update(
			database,
			"user_decks",
			deck,
			{
				db::Lookup("user_id", identity.userId),
				db::Lookup("deck_type", deck.deck_type),
				db::Lookup("deck_num", deck.deck_num),
				db::Lookup("member_type", deck.member_type),
				db::Lookup("disp_order", deck.disp_order),
			});
		// There's already an entry for this slot, so we're done.
		if (write.affected > 0)
		{
			affected += write.affected;
			continue;
		}

		// No existing entry for this slot, so insert a new one.
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
		"userinfo",
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
		"userinfo",
		{ db::Lookup("id", identity.userId) });
	auto packet = std::move(result.nonEmpty().front());

	packet.reinforcement_deck.emplace_back(0);
	packet.reinforcement_deck.emplace_back(0);
	packet.reinforcement_deck.emplace_back(0);
	packet.exp = 0;
	packet.add_unit_count = 100;

	// Derives the deck cost for a given user level based on the progression data.
	packet.deck_cost = 0;
	const auto& progression = theServer()->cache().initializeResp().progression;
	for (const auto& levelMst : progression)
	{
		if (levelMst.level == packet.level)
		{
			packet.deck_cost = levelMst.deck_cost;
		}
	}

	co_return db::InterfaceResult<UserTeamInfo>{
		.data = std::move(packet),
		.affected = result.affected,
	};
}

/*!
* Resolves and validates the current local user identity.
*
* The server stores one current Gumi Live user id, then maps it to the local
* userinfo id. Requests must send the same Gumi Live id, and normally must also
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
		{ db::Data("id", std::string()) })).front<std::string>("id");
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
		"userinfo",
		{
			db::Data("id", std::string()),
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
