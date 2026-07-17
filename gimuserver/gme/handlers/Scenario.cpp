#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/gme/common/Common.hpp>

#include <chrono>

// Cutscene viewed-state persistence.  The cutscene DATA is client-bundled
// (F_SCENARIO_MST: scenario id -> script file "mapN-*.txt" + BGM, reached via
// the CDN .dat pipeline — no MstResponse exists); the SERVER owns which
// scenarios this user has already watched, in the user_scenarios table.
//
// Flow: the client queries GetScenarioPlayingInfo during boot/scene entry and
// skips any cutscene whose id is in the returned sBbp47fi set; when a new
// cutscene finishes it reports the id via RaidUpScenarioInfo ("Up" = update —
// generic scenario reporter despite the Raid prefix).  The legacy fork
// stubbed both with {}, which is why cutscenes replayed on every boot.

// GetScenarioPlayingInfo (VRfsv4e3 / Bh4WqR01) — bare request (identity tags
// only); returns the viewed-scenario set.
HANDLEF(GetScenarioPlayingInfo)
{
	(void)session;
	LOG_INFO << "GetScenarioPlayingInfo: " << json;

	GetScenarioPlayingInfoResp resp = {};

	const std::string userId = co_await gme::getSoleUserId(theDb());
	if (!userId.empty())
	{
		const auto rows = co_await theDb()->execSqlCoro(
			"SELECT scenario_id FROM user_scenarios WHERE user_id = $1;", userId);
		for (const auto& row : rows)
		{
			resp.scenarios.push_back(::UserScenarioInfo{
				.scenario_id = row["scenario_id"].as<uint32_t>(),
			});
		}
	}

	std::string buffer{};
	if (const auto& ec = glz::write_json(resp, buffer); ec)
	{
		const auto& glze = glz::format_error(ec, buffer);
		LOG_DEBUG << "Gme GetScenarioPlayingInfo Error during JSON writing: " << glze;
		co_return HandleResult::error("Serialization error", glze);
	}

	co_return HandleResult::success(buffer);
}

// RaidUpScenarioInfo (R38qvphm / 72EyFbW8) — stores a viewed scenario id.
// Request: {"1ry6BKoe":[{"N4XVE1uA":"<scenario>"}]}.  The N4XVE1uA payload
// format is UNVERIFIED (expected: bare scenario id, quoted) — the INFO log
// below is the capture; non-numeric payloads are logged and skipped rather
// than guessed at (handbook §3.4 anti-pattern).
HANDLEF(RaidUpScenarioInfo)
{
	(void)session;
	LOG_INFO << "RaidUpScenarioInfo (CAPTURE — N4XVE1uA format unverified): " << json;

	RaidUpScenarioInfoReq req = {};
	if (const auto& ec = glz::read<glz::opts{ .error_on_unknown_keys = false }>(req, json); ec)
	{
		const auto& fmte = glz::format_error(ec, json);
		LOG_DEBUG << "Gme RaidUpScenarioInfo Error during JSON read: " << fmte;
		co_return HandleResult::error("Deserialization error", fmte);
	}

	const std::string userId = co_await gme::getSoleUserId(theDb());
	if (userId.empty())
	{
		co_return HandleResult::success("{}");
	}

	const auto now = static_cast<int64_t>(std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()).count());

	for (const auto& node : req.nodes)
	{
		if (node.scenario_info.empty())
			continue;

		uint32_t scenarioId = 0;
		try { scenarioId = static_cast<uint32_t>(std::stoul(node.scenario_info)); }
		catch (...)
		{
			LOG_WARN << "RaidUpScenarioInfo: non-numeric N4XVE1uA payload (composite "
				"format? capture above): " << node.scenario_info;
			continue;
		}

		try
		{
			co_await theDb()->execSqlCoro(
				"INSERT OR IGNORE INTO user_scenarios (user_id, scenario_id, viewed_at) "
				"VALUES ($1, $2, $3);",
				userId, scenarioId, now);
			LOG_INFO << "RaidUpScenarioInfo: stored viewed scenario " << scenarioId;
		}
		catch (const drogon::orm::DrogonDbException& ex)
		{
			LOG_ERROR << "RaidUpScenarioInfo: store failed for " << scenarioId
				<< ": " << ex.base().what();
		}
	}

	co_return HandleResult::success("{}");
}
