#include "App.hpp"
#include "Common.hpp"
#include "Handlers.hpp"

#include <gimuserver/archive/MissionArchiver.hpp>

#include <sstream>
#include <vector>

namespace
{
constexpr uint32_t FirstTutorialMission = 1;
constexpr uint32_t SecondTutorialMission = 2;

constexpr uint8_t FirstTutorialCheckpoint = 2;
constexpr uint8_t SecondTutorialCheckpoint = 10;

// TODO: After the second tutorial mission, we should set the number of
// gems to 5 to trigger the post mission cutscene.
constexpr uint8_t TutorialRewardGems = 5;

std::vector<UserUnitInfo> parseUnitDrops(const std::string& unitDrops)
{
	std::vector<UserUnitInfo> units;
	std::istringstream drops(unitDrops);
	for (std::string drop; std::getline(drops, drop, ',');)
	{
		if (drop.empty())
		{
			continue;
		}

		std::istringstream parts(drop);
		std::string id;
		std::string level;
		std::string type;
		if (!std::getline(parts, id, ':')
			|| !std::getline(parts, level, ':')
			|| !std::getline(parts, type, ':'))
		{
			LOG_ERROR << "Invalid mission drop unit entry: " << drop;
			return units;
		}

		auto unit = gme::fromArchivedUnit(
			static_cast<uint32_t>(std::stoul(id)),
			static_cast<uint32_t>(std::stoul(type)));
		if (!unit)
		{
			LOG_ERROR << "Unable to create mission drop unit from archive: " << drop;
			return units;
		}

		unit->unit_lvl = static_cast<uint32_t>(std::stoul(level));
		units.push_back(std::move(*unit));
	}

	return units;
}

std::string encodeUnitDrops(const std::vector<UserUnitInfo>& unitDrops)
{
	std::string encoded;
	for (const auto& drop : unitDrops)
	{
		if (!encoded.empty())
		{
			encoded += ',';
		}

		encoded += std::to_string(drop.unit_id)
			+ ':' + std::to_string(drop.user_unit_id)
			+ ':' + std::to_string(drop.unit_type_id)
			+ ":1";
	}

	return encoded;
}
}

HANDLEF(MissionEnd)
{
	MissionEndReq req = {};
	const auto& ec = glz::read<glz::opts{ .error_on_unknown_keys = false }>(req, json);
	if (ec)
	{
		const auto error = glz::format_error(ec, json);
		LOG_ERROR << "MissionEndReq deserialization failed:\n" << error;
		co_return HandleResult::error("Deserialization error", error);
	}

	auto identity = (co_await gme::getUserIdentity(theDb(), req.login_info)).data;
	MissionEndResp resp{};

	// This needs to be wrapped as a transcation to prevent cases where we issue
	// partial rewards upon mission completion.
	{
		auto transaction = co_await theDb()->newTransactionCoro();
		try
		{
			auto userInfo = co_await db::DatabaseInterface::read(
				transaction,
				"userinfo",
				{
					db::Data("level", int32_t()),
					db::Data("zel", int64_t()),
					db::Data("karma", int64_t()),
					db::Data("brave_coin", int32_t()),
					db::Lookup("gumi_user_id", identity.gumiUserId),
					db::Lookup("id", identity.userId),
			});
			const auto currentZel = userInfo.front<int64_t>("zel");
			const auto currentKarma = userInfo.front<int64_t>("karma");
			const auto currentLevel = userInfo.front<int32_t>("level");
			const auto currentBraveCoin = userInfo.front<int32_t>("brave_coin");

			// The client expects the post-mission total, including the amount earned
			// during this mission.
			(co_await db::DatabaseInterface::update(
				transaction,
				"userinfo",
				{
					db::Data("zel", currentZel + req.battle_result.zel),
					db::Data("karma", currentKarma + req.battle_result.karma),
					db::Lookup("gumi_user_id", identity.gumiUserId),
					db::Lookup("id", identity.userId)
				})).nonEmpty();

			// Persist tutorial checkpoints so leaving and returning mid-tutorial does not
			// replay completed steps.
			if (req.mission_num.serial_id == FirstTutorialMission)
			{
				(co_await db::DatabaseInterface::update(
					transaction,
					"userinfo",
					{
						db::Data("tutorial_status", FirstTutorialCheckpoint),
						db::Lookup("gumi_user_id", identity.gumiUserId),
						db::Lookup("id", identity.userId),
					})).nonEmpty();
			}
			else if (req.mission_num.serial_id == SecondTutorialMission)
			{
				(co_await db::DatabaseInterface::update(
					transaction,
					"userinfo",
					{
						db::Data("tutorial_status", SecondTutorialCheckpoint),
						db::Lookup("gumi_user_id", identity.gumiUserId),
						db::Lookup("id", identity.userId),
					})).nonEmpty();
			}

			auto loginInfo = std::move((co_await gme::getLoginInfo(transaction, identity)).nonEmpty());
			auto teamInfo = std::move((co_await gme::getTeamInfo(transaction, identity)).nonEmpty());

			auto droppedUnits = parseUnitDrops(req.battle_result.unit_rewards);
			for (auto& dropped : droppedUnits)
			{
				const auto userUnitId = (co_await db::PacketInterfaceFor<UserUnitInfo>::insert(
					transaction,
					"user_units",
					dropped,
					{ db::Data("user_id", identity.userId) })).front<uint32_t>("user_unit_id");

				dropped.user_unit_id = userUnitId;
			}

			auto unitInfo = std::move((co_await db::PacketInterfaceFor<UserUnitInfo>::read(
				transaction,
				"user_units",
				{ db::Lookup("user_id", identity.userId) })).nonEmpty());

			resp.reward_info.clear_mission_id = req.mission_num.serial_id;
			resp.reward_info.zel = req.battle_result.zel;
			resp.reward_info.karma = req.battle_result.karma;
			resp.reward_info.before_level = currentLevel;
			resp.reward_info.reward_units = encodeUnitDrops(droppedUnits);
			resp.login_info = std::move(loginInfo);
			resp.team_info = std::move(teamInfo);
			resp.unit_info = std::move(unitInfo);
		}
		catch (...)
		{
			transaction->rollback();
			throw;
		}
	}

	std::string buffer;
	const auto& writeError = glz::write_json(resp, buffer);
	if (writeError)
	{
		const auto& glze = glz::format_error(writeError, buffer);
		LOG_ERROR << "MissionEnd response serialization failed: " << glze;
		co_return HandleResult::error("Serialization error", glze);
	}

	co_return HandleResult::success(buffer);
}

HANDLEF(MissionStart)
{
	MissionStartReq req = {};
	const auto& ec = glz::read<glz::opts{ .error_on_unknown_keys = false }>(req, json);
	if (ec)
	{
		const auto error = glz::format_error(ec, json);
		LOG_ERROR << "MissionStartReq deserialization failed:\n" << error;
		co_return HandleResult::error("Deserialization error", error);
	}

	const auto missionRecord = MissionArchiver::instance().lookup(req.start_info.mission_id);
	if (!missionRecord)
	{
		co_return HandleResult::error("Archive error", "Unable to find mission record");
	}

	MissionStartResp resp{};
	resp.signal_key = req.signal_key;
	resp.start_info = req.start_info;

	if (!MissionArchiver::populatePacket(*missionRecord, resp.mission_num)
		|| !MissionArchiver::populatePacket(*missionRecord, resp.ais)
		|| !MissionArchiver::populatePacket(*missionRecord, resp.monsters)
		|| !MissionArchiver::populatePacket(*missionRecord, resp.battle_monster_groups)
		|| !MissionArchiver::populatePacket(*missionRecord, resp.battle_groups))
	{
		co_return HandleResult::error("Archive error", "Unable to populate mission start response");
	}

	std::string buffer;
	const auto& writeError = glz::write_json(resp, buffer);
	if (writeError)
	{
		const auto& glze = glz::format_error(writeError, buffer);
		LOG_ERROR << "MissionStart response serialization failed: " << glze;
		co_return HandleResult::error("Serialization error", glze);
	}

	co_return HandleResult::success(buffer);
}
