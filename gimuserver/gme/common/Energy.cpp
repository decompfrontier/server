#include "Energy.hpp"

#include "Common.hpp"

#include <gimuserver/db/DatabaseInterface.h>

#include <chrono>
#include <stdexcept>

namespace gme
{

uint32_t UserEnergy::derive(
	const uint32_t level,
	const uint64_t energyFullTs,
	uint32_t& energy)
{
	const auto mst = getLevelMst(level);
	if (!mst)
	{
		return 0;
	}

	// We allow overcapped energy.
	if (energy >= mst->energy)
	{
		return 0;
	}

	// Energy should have regenerated to full.
	const auto now = uint64_t(std::chrono::system_clock::to_time_t(
		std::chrono::system_clock::now()));
	if (now >= energyFullTs)
	{
		energy = mst->energy;
		return 0;
	}

	const auto secondsUntilFull = uint32_t(energyFullTs - now);
	const auto ticksUntilFull = (secondsUntilFull + kRegenSeconds - 1) / kRegenSeconds;
	const auto missingEnergy = ticksUntilFull * regenAmount(level);
	energy = mst->energy - missingEnergy;
	return secondsUntilFull;
}

drogon::Task<db::InterfaceResult<>> UserEnergy::consume(
	const db::Database database,
	const UserIdentity identity,
	const uint32_t cost)
{
	if (!database || identity.gumiUserId.empty() || identity.userId.empty())
	{
		LOG_ERROR << "Invalid UserEnergy::consume call: "
			<< "db=" << static_cast<bool>(database)
			<< ", gumi_user_id_empty=" << identity.gumiUserId.empty()
			<< ", user_id_empty=" << identity.userId.empty();
		throw std::invalid_argument("Invalid UserEnergy::consume call");
	}

	auto user = co_await db::DatabaseInterface::read(
		database,
		"user_info",
		{
			db::Data("level"),
			db::Data("energy"),
			db::Data("energy_full_ts"),
			db::Lookup("gumi_user_id", identity.gumiUserId),
			db::Lookup("id", identity.userId),
		});
	const auto level = user.front<uint32_t>("level");
	auto energy = user.front<uint32_t>("energy");
	auto energyFullTs = user.front<uint64_t>("energy_full_ts");

	// First, derive how much energy we currently have.
	derive(level, energyFullTs, energy);
	if (cost > energy)
	{
		LOG_ERROR << "Unable to spend " << cost
			<< " energy for user " << identity.userId
			<< ", current energy is " << energy;
		co_return db::InterfaceResult<>{};
	}

	const auto mst = getLevelMst(level);
	if (!mst)
	{
		co_return db::InterfaceResult<>{};
	}

	const auto now = uint64_t(std::chrono::system_clock::to_time_t(
		std::chrono::system_clock::now()));

	// Spend, then update the full timestamp based on the post-spend state.
	const auto newEnergy = energy - cost;

	// Still full or overcapped after spending, so no refill timer is needed.
	if (newEnergy >= mst->energy)
	{
		energyFullTs = 0;
	}
	// Started full or overcapped, but spending pushed us below cap. Start the
	// refill timer from now for only the missing energy.
	else if (energy >= mst->energy)
	{
		const auto missing = mst->energy - newEnergy;
		const auto ticks = (missing + regenAmount(level) - 1) / regenAmount(level);
		energyFullTs = now + ticks * kRegenSeconds;
	}
	// Already refilling, so preserve the current timer phase. At level 999+,
	// one tick restores 2 energy, so cost alone does not tell us how many
	// extra ticks this spend added. Compare the missing energy before and
	// after the spend instead.
	else
	{
		const auto before = mst->energy - energy;
		const auto after = mst->energy - newEnergy;
		const auto ticks =
			(after + regenAmount(level) - 1) / regenAmount(level)
			- (before + regenAmount(level) - 1) / regenAmount(level);
		energyFullTs += ticks * kRegenSeconds;
	}

	auto result = co_await db::DatabaseInterface::update(
		database,
		"user_info",
		{
			db::Data("energy", newEnergy),
			db::Data("energy_full_ts", energyFullTs),
			db::Lookup("gumi_user_id", identity.gumiUserId),
			db::Lookup("id", identity.userId),
		});

	co_return result;
}

drogon::Task<db::InterfaceResult<>> UserEnergy::refresh(
	const db::Database database,
	const UserIdentity identity,
	const uint32_t level,
	const bool overcap)
{
	if (!database || identity.gumiUserId.empty() || identity.userId.empty())
	{
		LOG_ERROR << "Invalid UserEnergy::refresh call: "
			<< "db=" << static_cast<bool>(database)
			<< ", gumi_user_id_empty=" << identity.gumiUserId.empty()
			<< ", user_id_empty=" << identity.userId.empty();
		throw std::invalid_argument("Invalid UserEnergy::refresh call");
	}

	const auto mst = getLevelMst(level);
	if (!mst)
	{
		co_return db::InterfaceResult<>{};
	}

	auto energy = mst->energy;

	// Overcap refresh adds the level cap on top of the user's derived
	// current energy instead of replacing it.
	if (overcap)
	{
		const auto user = co_await db::DatabaseInterface::read(
			database,
			"user_info",
			{
				db::Data("energy"),
				db::Data("energy_full_ts"),
				db::Lookup("gumi_user_id", identity.gumiUserId),
				db::Lookup("id", identity.userId),
			});

		// Derive current energy and overcap it.
		energy = user.front<uint32_t>("energy");
		const auto energyFullTs = user.front<uint64_t>("energy_full_ts");
		derive(level, energyFullTs, energy);
		energy = energy + mst->energy;
	}

	auto result = co_await db::DatabaseInterface::update(
		database,
		"user_info",
		{
			db::Data("energy", energy),
			db::Data("energy_full_ts", uint64_t(0)),
			db::Lookup("gumi_user_id", identity.gumiUserId),
			db::Lookup("id", identity.userId),
		});

	co_return result;
}

}
