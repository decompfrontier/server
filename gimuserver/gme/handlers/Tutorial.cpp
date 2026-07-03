#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/db/PacketInterface.hpp>
#include <gimuserver/gme/common/Common.hpp>
#include <cstdint>
#include <optional>

namespace
{

std::optional<uint32_t> getStarterUnit(uint32_t element)
{
	switch (element)
	{
	case 1:
		return 10011;
	case 2:
		return 20011;
	case 3:
		return 30011;
	case 4:
		return 40011;
	default:
		return std::nullopt;
	}
}
}

HANDLEF(NgwordCheck)
{
	co_return HandleResult::success("{}");
}

HANDLEF(CreateUser)
{
	CreateUserReq req = {};
	const auto& ec = glz::read_json(req, json);
	if (ec)
	{
		const auto error = glz::format_error(ec, json);
		LOG_ERROR << "CreateUserReq deserialization failed:\n" << error;
		co_return HandleResult::error("Deserialization error", error);
	}

	const auto& handleName = req.login_info.handle_name;
	if (handleName.empty())
	{
		co_return HandleResult::error(
			"Missing handle name",
			"CreateUser request did not include a handle name");
	}

	const auto identity = (co_await gme::getUserIdentity(theDb(), req.login_info, true)).data;
	if (!identity.userId.empty())
	{
		co_return HandleResult::error(
			"User already exists",
			"CreateUser cannot create a second user for this Gumi Live user");
	}
	const auto userId = RandomId();

	// Grab the starter units.
	const auto starterUnitId = getStarterUnit(req.selected_element.element);
	if (!starterUnitId)
	{
		co_return HandleResult::error("Invalid tutorial starter element");
	}
	auto starter = gme::fromArchivedUnit(*starterUnitId, UnitArchiver::getRandomType());
	auto burny = gme::fromArchivedUnit(10030, UnitArchiver::getRandomType());
	auto sparky = gme::fromArchivedUnit(40030, UnitArchiver::getRandomType());
	if (!starter || !burny || !sparky)
	{
		co_return HandleResult::error("Archive error", "Unable to create tutorial units from archive");
	}

	// We need to wrap these operations in a transaction to avoid an invalid
	// intermediate state. Keep this directly in the handler coroutine so local
	// packet objects remain alive across co_await suspension points.
	{
		auto transaction = co_await theDb()->newTransactionCoro();
		try
		{
			// Create the new user.
			const auto levelMst = gme::getLevelMst(1);
			if (!levelMst)
			{
				throw std::runtime_error("Unable to create user without level MST");
			}

			(co_await db::DatabaseInterface::insert(
				transaction,
				"user_info",
				{
					db::Data("id", userId),
					db::Data("gumi_user_id", identity.gumiUserId),
					db::Data("device_id", std::string()),
					db::Data("username", handleName),
					db::Data("level", uint32_t(1)),
					db::Data("zel", uint64_t(5000)),
					db::Data("karma", uint64_t(1000)),
					db::Data("energy", levelMst->energy),
					db::Data("energy_full_ts", uint64_t(0)),
					db::Data("max_warehouse_count", 100),
				})).nonEmpty();

			starter->user_unit_id = (co_await db::PacketInterfaceFor<UserUnitInfo>::insert(
				transaction,
				"user_units",
				*starter,
				{ db::Data("user_id", userId) })).front<uint32_t>("user_unit_id");

			(co_await gme::addDefaultDecks(
				transaction,
				{
					.gumiUserId = identity.gumiUserId,
					.userId = userId,
				},
				*starter)).nonEmpty();

			burny->user_unit_id = (co_await db::PacketInterfaceFor<UserUnitInfo>::insert(
				transaction,
				"user_units",
				*burny,
				{ db::Data("user_id", userId) })).front<uint32_t>("user_unit_id");

			(co_await db::PacketInterfaceFor<UserPartyDeckInfo>::insert(
				transaction,
				"user_decks",
				{
					.user_unit_id = burny->user_unit_id,
					.deck_type = 1,
					.deck_num = 0,
					.member_type = 1,
					.disp_order = 0,
				},
				{ db::Data("user_id", userId) })).nonEmpty();

			sparky->user_unit_id = (co_await db::PacketInterfaceFor<UserUnitInfo>::insert(
				transaction,
				"user_units",
				*sparky,
				{ db::Data("user_id", userId) })).front<uint32_t>("user_unit_id");

			(co_await db::PacketInterfaceFor<UserPartyDeckInfo>::insert(
				transaction,
				"user_decks",
				{
					.user_unit_id = sparky->user_unit_id,
					.deck_type = 1,
					.deck_num = 0,
					.member_type = 1,
					.disp_order = 1,
				},
				{ db::Data("user_id", userId) })).nonEmpty();
		}
		catch (...)
		{
			transaction->rollback();
			throw;
		}
	}

	co_return HandleResult::success("{}");
}

HANDLEF(TutorialSkip)
{
	// Client just needs an acknowledgment.
	TutorialSkipResp resp{};
	resp.tutorial_skip_info.ack = true;

	std::string buffer{};
	const auto& ec = glz::write_json(resp, buffer);
	if (ec)
	{
		co_return HandleResult::error("Serialization error");
	}

	co_return HandleResult::success(buffer);
}

HANDLEF(TutorialUpdate)
{
	TutorialUpdateReq req = {};
	const auto& ec = glz::read_json(req, json);
	if (ec)
	{
		const auto error = glz::format_error(ec, json);
		LOG_ERROR << "TutorialUpdateReq deserialization failed:\n" << error;
		co_return HandleResult::error("Deserialization error", error);
	}

	const auto identity = (co_await gme::getUserIdentity(theDb(), req.login_info)).nonEmpty();

	(co_await db::DatabaseInterface::update(
		theDb(),
		"user_info",
		{
			db::Data("tutorial_status", req.login_info.tutorial_status),
			db::Lookup("gumi_user_id", identity.gumiUserId),
			db::Lookup("id", identity.userId),
		})).nonEmpty();

	co_return HandleResult::success("{}");
}
