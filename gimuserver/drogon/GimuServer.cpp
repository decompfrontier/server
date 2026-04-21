#include "App.hpp"
#include "GimuServer.hpp"

#include <random>

GimuServer::GimuServer() : m_dlc_error_log(), m_have_log(false), m_cache() {}

void GimuServer::initAndStart(const Json::Value& config)
{
	const auto& extralog = config["extra_log"];
	if (extralog["enable"].asBool())
	{
		std::string dir = extralog["directory"].asCString();
		std::string log_prefix = extralog["http_log_prefix"].asString();

		std::filesystem::path logPrefixPath(dir);
		logPrefixPath /= log_prefix;
		m_http_log_prefix = logPrefixPath.string();

		m_http_log_suffix = extralog["http_log_suffix"].asString();
		std::string dlcLogName = extralog["dlc_error_log_name"].asString();

		if (!std::filesystem::is_directory(dir))
		{
			if (!std::filesystem::create_directory(dir))
			{
				LOG_WARN << "Cannot create log directory!";
				m_have_log = false;
			}
			else
			{
				m_have_log = true;
			}
		}
		else
		{
			m_have_log = true;
		}

		if (m_have_log)
		{
			std::filesystem::path dlcPath(dir);
			dlcPath /= dlcLogName;
			m_dlc_error_log.open(dlcPath.string());
		}
	}

	const auto& server = config["server"];
	m_cache.Setup(server);
}

/*!
* Maps a UnitMst element integer to the wire-format string expected by the client.
*/
static std::string_view ElementIdToString(int32_t id)
{
	switch (id)
	{
	case 1:  return "fire";
	case 2:  return "water";
	case 3:  return "earth";
	case 4:  return "thunder";
	case 5:  return "light";
	case 6:  return "dark";
	default: return "fire";
	}
}

void GimuServer::SeedDefaultUnits(drogon::orm::DbClientPtr db)
{
	const std::string userId = "0839899613932562";

	const auto& mst = m_cache.unitMst();
	if (mst.empty())
	{
		LOG_WARN << "SeedDefaultUnits: UnitMst is empty, skipping unit seed";
		return;
	}

	// Idempotency check — skip if this user already owns units
	const auto countRes = db->execSqlSync(
		"SELECT COUNT(*) FROM user_units WHERE user_id=$1;", userId);
	if (countRes[0][0].as<int64_t>() > 0)
	{
		LOG_INFO << "SeedDefaultUnits: units already present, skipping";
		return;
	}

	// Randomise unit_type_id (1-6: Lord/Anima/Breaker/Guardian/Oracle/Rex)
	// so that stat changes are visually distinguishable across the inventory.
	std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<int32_t> typeDist(1, 6);

	int seeded = 0;
	for (const auto& unit : mst)
	{
		if (unit.id == 1)  continue; // skip summoner NPC
		if (seeded >= 100) break;

		db->execSqlSync(
			"INSERT INTO user_units "
			"(user_id, unit_id, unit_lv,"
			" base_hp,  add_hp,  ext_hp,  limit_over_hp,"
			" base_atk, add_atk, ext_atk, limit_over_atk,"
			" base_def, add_def, ext_def, limit_over_def,"
			" base_heal,add_heal,ext_heal,limit_over_heal,"
			" exp, total_exp,"
			" skill_id, skill_lv, extra_skill_id, extra_skill_lv, leader_skill_id,"
			" element, fe_bp, fe_max_usable_bp, unit_type_id) "
			"VALUES ($1,$2,1,"
			" $3,0,0,0, $4,0,0,0, $5,0,0,0, $6,0,0,0,"
			" 1,1,"
			" $7,0,$8,0,$9,"
			" $10,100,200,$11);",
			userId, std::to_string(unit.id),
			unit.min_hp, unit.min_atk, unit.min_def, unit.min_rec,
			unit.skill_id, unit.extra_skill_id, unit.leader_skill_id,
			std::string(ElementIdToString(unit.element)),
			typeDist(rng)
		);
		++seeded;
	}

	LOG_INFO << "SeedDefaultUnits: seeded " << seeded << " units for user " << userId;
}

void GimuServer::shutdown() {}
