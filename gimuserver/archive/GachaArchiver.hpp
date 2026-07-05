#pragma once

#include <gimuserver/archive/archive.hpp>
#include <gimuserver/packets/all.hpp>

#include <json/value.h>

#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

/*!
* Loads and serves server-owned curated summon gate archive records.
*
* Gacha records only store the fields the server wants to override. The
* archiver copies the remaining GachaInfoMst fields from the official GachaMst
* row with the same id.
*/
class GachaArchiver final
{
public:
	using GachaId = uint32_t;

	/*!
	* Gets the process-wide gacha archiver.
	*
	* Setup() must be called during server initialization before request handlers
	* depend on archive data.
	*/
	static GachaArchiver& instance();

	/*!
	* Loads gacha archive records from archive_root/gacha.json and official
	* summon gate MST rows from mst_root/gacha.json.
	*
	* @param serverObj Server configuration object from the Drogon plugin config.
	*/
	void setup(const Json::Value& serverObj);

	/*!
	* Looks up a gacha archive record.
	*
	* @param gacha_id Summon gate id from the gacha archive.
	* @return Matching archive record, or std::nullopt when not found.
	*/
	std::optional<GachaRecord> lookup(GachaId gacha_id) const;

	/*!
	* Populates one GachaInfoMst row from a gacha archive record.
	*
	* @param record Archive record to read from.
	* @param mst Packet row to populate.
	* @return True if the packet row was populated, false if the official
	* GachaMst row is missing.
	*/
	bool populatePacket(const GachaRecord& record, GachaInfoMst& mst) const;

	/*!
	* Rolls unit ids from a summon gate archive record.
	*
	* @param record Archive record to summon from.
	* @param count Number of units to summon.
	* @return Selected unit archive ids.
	*/
	std::vector<uint32_t> summonFrom(const GachaRecord& record, uint32_t count) const;

	/*!
	* Populates all archived summon gate info rows.
	*
	* Note that since we store cache as unordered maps, the order of the 
	* populated rows is not guaranteed to be the same as the order in the archive.
	*
	* @return All archived gacha info rows.
	*/
	std::vector<GachaInfoMst> populateAllPackets() const;

	GachaArchiver(const GachaArchiver&) = delete;
	GachaArchiver& operator=(const GachaArchiver&) = delete;

private:
	using GachaRecordCache = std::unordered_map<GachaId, GachaRecord>;
	using GachaMstMap = std::unordered_map<GachaId, GachaMst>;

	GachaArchiver() = default;

	GachaRecordCache cache_;
	GachaMstMap mstCache_;
};
