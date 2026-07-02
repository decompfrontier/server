#include "App.hpp"
#include "GimuServer.hpp"

#include <random>

#include <gimuserver/archive/MissionArchiver.hpp>
#include <gimuserver/archive/UnitArchiver.hpp>

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
	UnitArchiver::instance().setup(server);
	MissionArchiver::instance().setup(server);
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

void GimuServer::InsertUnitFromMst(drogon::orm::DbClientPtr db,
                                   const std::string& userId,
                                   const UnitMst& unit)
{
	// Randomise unit_type_id (1-6: Lord/Anima/Breaker/Guardian/Oracle/Rex)
	// so that stat changes are visually distinguishable across the inventory.
	static thread_local std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<int32_t> typeDist(1, 6);

	// skill_lv must be > 0 for the battle engine to allow BB activation.
	// Match the old-tree convention: level 10 for any unit that has a skill,
	// 0 if the unit has no skill assigned (skill_id == 0).
	const int32_t skillLv      = unit.skill_id       > 0 ? 10 : 0;
	const int32_t extraSkillLv = unit.extra_skill_id > 0 ? 10 : 0;

	// Parameters are strictly sequential ($1..$13) to make the mapping
	// unambiguous regardless of how the underlying driver handles $N indices:
	// $1=user_id, $2=unit_id, $3=hp, $4=atk, $5=def, $6=rec,
	// $7=skill_id, $8=skill_lv, $9=extra_skill_id, $10=extra_skill_lv,
	// $11=leader_skill_id, $12=element, $13=unit_type_id.
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
		" $7,$8,$9,$10,$11,"
		" $12,100,200,$13);",
		userId, std::to_string(unit.id),
		unit.min_hp, unit.min_atk, unit.min_def, unit.min_rec,
		unit.skill_id, skillLv, unit.extra_skill_id, extraSkillLv, unit.leader_skill_id,
		std::string(ElementIdToString(unit.element)),
		typeDist(rng)
	);
}


void GimuServer::shutdown() {}
