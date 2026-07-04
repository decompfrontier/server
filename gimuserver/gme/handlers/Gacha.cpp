#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/archive/GachaArchiver.hpp>
#include <gimuserver/gme/common/Common.hpp>
#include <gimuserver/utils/Random.hpp>

#include <algorithm>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>

namespace
{

/*!
* Picks a summon animation effect id for a summoned unit rarity.
*
* @param rarity Unit rarity used to choose the animation effect.
* @return Gacha effect id to send, or std::nullopt if none is available.
*/
inline std::optional<uint32_t> getGachaEffect(uint32_t rarity)
{
	const auto& mst = theServer()->cache().initializeResp().gacha_effects;

	uint32_t totalWeight = 0;
	std::vector<const GachaEffectMst*> effects;
	// The MST is already sorted by rarity.
	for (auto it = mst.begin(); it != mst.end() && it->rarity <= rarity; ++it)
	{
		if (it->rarity == rarity && it->weight > 0)
		{
			effects.push_back(&*it);
			totalWeight += it->weight;
		}
	}

	if (effects.empty())
	{
		LOG_ERROR << "Unable to find gacha effect for rarity " << rarity;
		return std::nullopt;
	}

	auto roll = RandomUInt(1, totalWeight);
	for (const auto& effect : effects)
	{
		if (roll <= effect->weight)
		{
			return effect->id;
		}
		roll -= effect->weight;
	}

	return std::nullopt;
}

} // namespace

HANDLEF(GachaAction)
{
	GachaActionReq req{};
	const auto& ec = glz::read_json(req, json);
	if (ec)
	{
		const auto& fmte = glz::format_error(ec, json);
		LOG_DEBUG << "Gme GachaAction Error during JSON read: " << fmte;
		co_return HandleResult::error("Deserialization error", fmte);
	}

	const auto identity = (co_await gme::getUserIdentity(theDb(), req.login_info)).nonEmpty();

	// TODO: Implement summon tickets. For now, just reject them.
	if (req.gacha_action_info.using_gacha_ticket)
	{
		co_return HandleResult::error("Unsupported", "Summon tickets are not implemented");
	}

	// Obviously, we can't summon 0 times.
	const auto count = req.gacha_action_info.request_count;
	if (count == 0)
	{
		co_return HandleResult::error("Invalid request count", "Request count must be at least 1");
	}

	// Find the summon gate and determine the cost.
	const auto gachaRecord = GachaArchiver::instance().lookup(req.gacha_action_info.gacha_id);
	if (!gachaRecord)
	{
		co_return HandleResult::error("Invalid gacha", "Unknown summon gate");
	}

	GachaInfoMst mst{};
	if (!GachaArchiver::instance().populatePacket(*gachaRecord, mst))
	{
		co_return HandleResult::error("Invalid gacha", "Missing in MST");
	}

	const auto gemCost = mst.gems * count;
	const auto friendPointCost = mst.friend_points * count;

	const auto summonedUnits = GachaArchiver::instance().summonFrom(*gachaRecord, count);
	if (summonedUnits.size() != count)
	{
		co_return HandleResult::error("Invalid gacha", "Unable to summon from gate");
	}

	// Ensure the user has enough currency to perform the summon.
	auto currency = co_await db::DatabaseInterface::read(
		theDb(),
		"user_info",
		{
			db::Data("gems"),
			db::Data("friend_points"),
			db::Lookup("gumi_user_id", identity.gumiUserId),
			db::Lookup("id", identity.userId),
		});
	const auto currentGems = currency.front<int32_t>("gems");
	const auto currentFriendPoints = currency.front<int32_t>("friend_points");
	if (gemCost > currentGems || friendPointCost > currentFriendPoints)
	{
		co_return HandleResult::error("Invalid gacha request", "Not enough currency");
	}

	// We need to wrap these operations in a transaction to avoid an invalid
	// intermediate state.
	GachaActionResp resp{};
	resp.signal_key = req.signal_key;
	{
		auto transaction = co_await theDb()->newTransactionCoro();
		try
		{
			// Deduct the currency for the summon.
			(co_await db::DatabaseInterface::update(
				transaction,
				"user_info",
				{
					db::Data("gems", currentGems - gemCost),
					db::Data("friend_points", currentFriendPoints - friendPointCost),
					db::Lookup("gumi_user_id", identity.gumiUserId),
					db::Lookup("id", identity.userId),
				})).nonEmpty();

			// Commit the summoned units and populate the response.
			for (const auto& unitId : summonedUnits)
			{
				// Build and inject the unit.
				const auto unitRecord = UnitArchiver::instance().lookup(unitId);
				if (!unitRecord)
				{
					throw std::runtime_error("Unable to find summoned unit archive record");
				}

				auto unit = gme::fromArchivedUnit(
					unitId,
					UnitArchiver::getRandomType());
				if (!unit)
				{
					throw std::runtime_error("Unable to create summoned unit from archive");
				}

				auto added = std::move(
					(co_await gme::addUserUnit(transaction, identity, *unit)).nonEmpty());
				resp.unit_dictionary.push_back(std::move(
					(co_await db::PacketInterfaceFor<UserUnitDictionary>::read(
						transaction,
						"user_unit_dictionary",
						{
							db::Lookup("user_id", identity.userId),
							db::Lookup("unit_id", unitId),
						})).nonEmpty().front()));

				const auto gachaEffect = getGachaEffect(unitRecord->rarity);
				if (!gachaEffect)
				{
					throw std::runtime_error(
						"Unable to find gacha effect for summoned unit rarity");
				}

				resp.ope_user_unit.push_back({
					.user_unit_id = added.user_unit_id,
					.gacha_effect_id = *gachaEffect,
				});
				resp.unit_info.push_back(std::move(added));
			}

			resp.team_info = std::move((co_await gme::getTeamInfo(transaction, identity)).nonEmpty());
		}
		catch (...)
		{
			transaction->rollback();
			throw;
		}
	}

	std::string buffer;
	const auto& ec2 = glz::write_json(resp, buffer);
	if (ec2)
	{
		const auto& glze = glz::format_error(ec2, buffer);
		LOG_DEBUG << "Gme GachaAction Error during JSON writing: " << glze;
		co_return HandleResult::error("Serialization error", glze);
	}

	co_return HandleResult::success(buffer);
}

HANDLEF(GachaList)
{
	GachaListReq req = {};
	const auto& ec = glz::read_json(req, json);
	if (ec)
	{
		const auto& fmte = glz::format_error(ec, json);
		LOG_DEBUG << "Gme GachaList Error during JSON read: " << fmte;
		co_return HandleResult::error("Deserialization error", fmte);
	}

	// Just reuse the static response that we've cached.
	GachaListResp resp = theServer()->cache().gachaListRsp();
	resp.gacha_info = GachaArchiver::instance().populateAllPackets();
	resp.signal_key = req.signal_key;

	std::string buffer;
	const auto& ec2 = glz::write_json(resp, buffer);
	if (ec2)
	{
		const auto& glze = glz::format_error(ec2, buffer);
		LOG_DEBUG << "Gme GachaList Error during JSON writing: " << glze;
		co_return HandleResult::error("Serialization error", glze);
	}

	co_return HandleResult::success(buffer);
}
