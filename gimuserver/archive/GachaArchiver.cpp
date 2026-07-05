#include "GachaArchiver.hpp"

#include "UnitArchiver.hpp"

#include <gimuserver/drogon/ServerCacheMst.hpp>
#include <gimuserver/utils/JsonFile.hpp>
#include <gimuserver/utils/Random.hpp>

#include <drogon/drogon.h>

#include <algorithm>
#include <exception>
#include <string>
#include <utility>
#include <vector>

GachaArchiver& GachaArchiver::instance()
{
	static GachaArchiver instance;
	return instance;
}

void GachaArchiver::setup(const Json::Value& serverObj)
{
	LOG_INFO << "Setting up gacha archiver cache. "
		"If you see this after initialization, it is a bug.";

	const auto archiveRoot = serverObj["archive_root"].asString();
	const auto mstRoot = serverObj["mst_root"].asString();
	if (archiveRoot.empty() || mstRoot.empty())
	{
		LOG_ERROR << "Unable to set up gacha archiver cache: "
			<< "archive_root_empty=" << archiveRoot.empty()
			<< ", mst_root_empty=" << mstRoot.empty();
		return;
	}

	std::vector<GachaRecord> records;
	std::vector<GachaMst> gachas;
	try
	{
		records = LoadJson<std::vector<GachaRecord>>(archiveRoot, "gacha.json");
		gachas = LoadJson<GachaMstCache>(mstRoot, "gacha.json").data;
	}
	catch (const std::exception& ex)
	{
		LOG_ERROR << "Unable to set up gacha archiver cache: " << ex.what();
		return;
	}

	cache_.clear();
	cache_.reserve(records.size());
	for (auto& record : records)
	{
		cache_.insert_or_assign(record.id, std::move(record));
	}

	mstCache_.clear();
	mstCache_.reserve(gachas.size());
	for (auto& gacha : gachas)
	{
		mstCache_.insert_or_assign(gacha.id, std::move(gacha));
	}

	LOG_INFO << "Loaded " << cache_.size()
		<< " gacha archive records from " << archiveRoot << "/gacha.json";
}

std::optional<GachaRecord> GachaArchiver::lookup(GachaId gacha_id) const
{
	const auto it = cache_.find(gacha_id);
	if (it == cache_.end())
	{
		LOG_ERROR << "Unable to find gacha archive record " << gacha_id;
		return std::nullopt;
	}

	return it->second;
}

bool GachaArchiver::populatePacket(const GachaRecord& record, GachaInfoMst& mst) const
{
	const auto it = mstCache_.find(record.id);
	if (it == mstCache_.end())
	{
		LOG_ERROR << "Unable to populate gacha info: missing GachaMst row "
			<< record.id;
		return false;
	}

	const auto& gachaMst = it->second;
	mst.id = gachaMst.id;
	mst.gems = gachaMst.need_gems;
	mst.button = record.button.value_or(gachaMst.btn_img);
	mst.background = record.background.value_or(gachaMst.bg_img);
	mst.door = record.door.value_or(gachaMst.door_img);
	mst.caption_msg = record.caption_msg.value_or(gachaMst.caption_msg);
	mst.detail_msg = record.detail_msg.value_or(gachaMst.detail_msg);
	mst.name = gachaMst.name;
	mst.type = gachaMst.type;
	mst.priority = record.priority;
	mst.friend_points = gachaMst.need_friend_point;
	mst.group_id = gachaMst.gacha_group_id;

	return true;
}

std::vector<uint32_t> GachaArchiver::summonFrom(const GachaRecord& record, uint32_t count) const
{
	uint32_t total = 0;
	std::vector<const GachaPull*> pulls;
	pulls.reserve(record.pool.size());
	for (const auto& pull : record.pool)
	{
		if (pull.weight > 0)
		{
			pulls.push_back(&pull);
			total += pull.weight;
		}
	}

	if (pulls.empty())
	{
		LOG_ERROR << "Unable to summon from gacha archive record "
			<< record.id << ": empty unit pool";
		return {};
	}

	std::vector<uint32_t> units;
	units.reserve(count);
	for (uint32_t i = 0; i < count; ++i)
	{
		auto roll = RandomUInt(1, total);
		for (const auto& pull : pulls)
		{
			if (roll <= pull->weight)
			{
				units.push_back(pull->unit_id);
				break;
			}
			roll -= pull->weight;
		}
	}

	// Client only shows animation effects for the last unit, so make sure
	// it's the one with the highest rarity.
	std::sort(units.begin(), units.end(), [](uint32_t lhs, uint32_t rhs) {
		const auto left = UnitArchiver::instance().lookup(lhs);
		const auto right = UnitArchiver::instance().lookup(rhs);
		if (!left || !right)
		{
			throw std::runtime_error("Unable to sort summoned units by rarity");
		}

		return left->rarity < right->rarity;
	});

	return units;
}

std::vector<GachaInfoMst> GachaArchiver::populateAllPackets() const
{
	std::vector<const GachaRecord*> records;
	records.reserve(cache_.size());
	for (const auto& [_, record] : cache_)
	{
		records.push_back(&record);
	}

	std::vector<GachaInfoMst> msts;
	msts.reserve(records.size());
	for (const auto& record : records)
	{
		GachaInfoMst mst;
		if (populatePacket(*record, mst))
		{
			msts.push_back(std::move(mst));
		}
	}

	return msts;
}
