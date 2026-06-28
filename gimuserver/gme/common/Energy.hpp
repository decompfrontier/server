#pragma once

#include <gimuserver/db/Types.h>

#include <drogon/drogon.h>

#include <cstdint>

namespace gme
{
struct UserIdentity;

/*!
* Handles persisted user energy and client-facing refill countdowns.
*/
class UserEnergy
{
public:
	/*!
	* Derives the current energy value and the countdown expected by the client.
	*
	* @param level Current player level used to look up max energy.
	* @param energyFullTs Stored Unix timestamp for when energy reaches full.
	* @param energy Stored energy value. Mutated to the derived current energy.
	* @return Seconds until energy is full for UserTeamInfo::energy_full_seconds.
	*/
	static uint32_t derive(uint32_t level, uint64_t energyFullTs, uint32_t& energy);

	/*!
	* Spends user energy and persists the updated refill state.
	*
	* Consuming energy extends energy_full_ts by the number of regen ticks
	* needed to recover the consumed amount.
	*
	* @param database Database client or transaction to use.
	* @param identity Resolved user identity that owns the energy row.
	* @param cost Energy amount to consume.
	* @return Number of affected rows.
	*/
	static drogon::Task<db::InterfaceResult<>> consume(
		db::Database database,
		UserIdentity identity,
		uint32_t cost);

	/*!
	* Refreshes user energy and clears the refill timer.
	*
	* Refreshing sets the stored energy to full, or overcap-full, and sets
	* energy_full_ts to zero.
	*
	* @param database Database client or transaction to use.
	* @param identity Resolved user identity that owns the energy row.
	* @param level Player level used to look up max energy.
	* @param overcap True to add max energy on top of derived current energy.
	* @return Number of affected rows.
	*/
	static drogon::Task<db::InterfaceResult<>> refresh(
		db::Database database,
		UserIdentity identity,
		uint32_t level,
		bool overcap=false);

private:
	// Seconds required to regenerate one energy tick.
	static constexpr uint32_t kRegenSeconds = 180;

	static uint32_t regenAmount(const uint32_t level)
	{
		return level >= 999 ? 2 : 1;
	}
};
}
