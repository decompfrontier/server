#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/gme/common/Common.hpp>
#include <cmath>

// UnitMix (Power Fusion) — fuse material units into a base unit, gaining exp.
//
// Request  (group Mw08CIg2, key JnegC7RrN3FoW8dQ):
//   "60subGk3": [{"81GjwoWy":"1","2vnqRIr3":"2"}]  — mix/ingredient type (log only)
//   "mCE3rUu5": [{"Rs7bCE3t":"<zelCost>"}]          — zel cost (string)
//   "Km35HAXv": [{"edy7fq3L":"<id>","mnZ5K4Ii":"1"}, // base unit  (role 1)
//                {"edy7fq3L":"<id>","mnZ5K4Ii":"2"}, // material   (role 2) x N]
//
// Response:
//   "xZH6EIQ7": [UnitReinforceEntry] — drives the level-up animation
//   "qC2tJs4E": [UserUnitInfo]       — incremental unit cache update
//   "fEi17cnx": [UserTeamInfo]       — updated zel

// The request struct (UnitMixReq + UnitMixUnitEntry/UnitMixZelEntry) is
// generated from packet-generator/assets/net/{handlers,unit}.kdl.

// The response struct (UnitMixResp + the shared UnitReinforceEntry under
// xZH6EIQ7) is generated from packet-generator/assets/net/{handlers,unit}.kdl.
// unit_update rides UserUnitInfo under qC2tJs4E; team_info rides UserTeamInfo
// under fEi17cnx.

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
static std::string unitMix_stripSuffix(const std::string& raw)
{
    auto pos = raw.find('_');
    return (pos != std::string::npos) ? raw.substr(0, pos) : raw;
}

// Cumulative exp needed to REACH `level` from level 1.
// UnitExpPatternMst::need_exp is the incremental cost per level transition.
static int unitMix_expForLevel(const std::vector<UnitExpPatternMst>& pat,
                                int patternId, int level)
{
    int acc = 0;
    for (const auto& e : pat)
    {
        if (e.id != patternId) continue;
        if (e.lv <= 1)         continue;
        if (e.lv > level)      break;
        acc += e.need_exp;
    }
    return acc;
}

// Highest level reachable with totalExp under maxLevel cap.
static int unitMix_levelFromExp(const std::vector<UnitExpPatternMst>& pat,
                                 int patternId, int maxLevel, int totalExp)
{
    int acc = 0, level = 1;
    for (const auto& e : pat)
    {
        if (e.id != patternId)   continue;
        if (e.lv <= 1)           continue;
        if (e.lv > maxLevel)     break;
        if (acc + e.need_exp <= totalExp) { acc += e.need_exp; level = e.lv; }
        else break;
    }
    return level;
}

// ---------------------------------------------------------------------------
// Handler
// ---------------------------------------------------------------------------
HANDLEF(UnitMix)
{
    (void)session;
    LOG_INFO << "UnitMix: " << json;

    // Parse request.  Use error_on_unknown_keys=false so the extra "60subGk3"
    // operation-type group sent by the client doesn't abort parsing.
    UnitMixReq req = {};
    {
        glz::context ctx{};
        if (const auto& ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(req, json, ctx); ec)
        {
            LOG_WARN << "UnitMix: bad request JSON: " << glz::format_error(ec, json);
            co_return HandleResult::error("Deserialization error");
        }
    }

    // Resolve the current user from the request's login info.
    const auto identity = (co_await gme::getUserIdentity(theDb(), req.login_info)).nonEmpty();
    const std::string kUserId = identity.userId;

    // Split base vs material units.
    int32_t baseId = 0;
    std::vector<int32_t> matIds;
    for (const auto& u : req.units)
    {
        if (u.role == "1")       baseId = u.user_unit_id;
        else if (u.user_unit_id) matIds.push_back(u.user_unit_id);
    }

    if (baseId == 0)
    {
        LOG_WARN << "UnitMix: no base unit in request";
        co_return HandleResult::error("UnitMix: no base unit");
    }

    const int32_t zelCost = req.zel_cost_list.empty() ? 0 : req.zel_cost_list[0].cost;

    // Build material IN-clause.
    std::string matList;
    for (size_t i = 0; i < matIds.size(); ++i)
    {
        if (i) matList += ',';
        matList += std::to_string(matIds[i]);
    }

    // Step 1: SELECT base unit full stats.
    const auto baseRows = co_await theDb()->execSqlCoro(
        "SELECT user_unit_id, unit_id, total_exp,"
        " base_hp, base_atk, base_def, base_heal,"
        " add_hp, add_atk, add_def, add_heal,"
        " ext_hp, ext_atk, ext_def, ext_heal,"
        " limit_over_hp, limit_over_atk, limit_over_def, limit_over_heal,"
        " skill_id, skill_lv, extra_skill_id, extra_skill_lv, leader_skill_id,"
        " element, fe_bp, fe_max_usable_bp, unit_type_id,"
        " eqip_item_id, eqip_item_frame_id, eqip_item_id2, eqip_item_frame_id2"
        " FROM user_units WHERE user_id=$1 AND user_unit_id=$2 LIMIT 1;",
        std::string(kUserId), baseId
    );

    if (baseRows.empty())
    {
        LOG_WARN << "UnitMix: base unit " << baseId << " not found";
        co_return HandleResult::error("UnitMix: base unit not found");
    }
    const auto& br = baseRows[0];

    const std::string rawBaseUnitId = br["unit_id"].as<std::string>();
    const std::string baseMstId     = unitMix_stripSuffix(rawBaseUnitId);
    const int32_t     baseMstIdInt  = std::stoi(baseMstId);
    const int         baseTotalExp  = br["total_exp"].as<int32_t>();

    // Lookup base unit MST data.
    const auto& unitMst = theServer()->cache().unitMst();
    const UnitMst* baseMstData = nullptr;
    for (const auto& u : unitMst) { if (u.id == baseMstIdInt) { baseMstData = &u; break; } }

    const int baseElement   = baseMstData ? baseMstData->element       : 0;
    const int expPatternId  = baseMstData ? baseMstData->exp_pattern_id: 10;
    const int maxLevel      = baseMstData ? baseMstData->max_lv        : 100;

    // Step 2: SELECT material stats to compute exp gain.
    float gainedExpF = 0.0f;
    if (!matIds.empty())
    {
        static const int kRarityBonus[] = { 0, 100, 200, 500, 1000, 1500, 3000, 5000, 10000 };

        const auto matRows = co_await theDb()->execSqlCoro(
            "SELECT unit_id, total_exp FROM user_units"
            " WHERE user_id=$1 AND user_unit_id IN (" + matList + ");",
            std::string(kUserId)
        );

        for (const auto& row : matRows)
        {
            const std::string matMstId    = unitMix_stripSuffix(row["unit_id"].as<std::string>());
            const int32_t     matMstIdInt = std::stoi(matMstId);
            const int         matTotalExp = row["total_exp"].as<int32_t>();

            const UnitMst* matData = nullptr;
            for (const auto& u : unitMst) { if (u.id == matMstIdInt) { matData = &u; break; } }

            const int matAdjust = matData ? matData->adjust_exp : 0;
            const int matCost   = matData ? matData->cost       : 1;
            const int matRare   = matData ? matData->rarity     : 1;
            const int matElem   = matData ? matData->element    : 0;

            float matExp = (float)matTotalExp / 2.5f;
            matExp += (float)(matCost * 2);
            matExp += (float)matAdjust;
            if (matRare >= 1 && matRare <= 8)
                matExp += (float)kRarityBonus[matRare];
            if (baseElement != 0 && matElem == baseElement)
                matExp *= 1.5f;

            gainedExpF += matExp;
        }
    }

    const int gainedExp = (int)llroundf(gainedExpF);

    // Compute new level / exp.
    const auto& expPat      = theServer()->cache().initializeResp().exp_pattern;
    const int   maxTotalExp = unitMix_expForLevel(expPat, expPatternId, maxLevel);
    int         newTotalExp = baseTotalExp + gainedExp;
    if (maxTotalExp > 0 && newTotalExp > maxTotalExp) newTotalExp = maxTotalExp;

    const int newLevel    = unitMix_levelFromExp(expPat, expPatternId, maxLevel, newTotalExp);
    const int levelExpFlr = unitMix_expForLevel(expPat, expPatternId, newLevel);
    const int newExp      = newTotalExp - levelExpFlr;

    LOG_INFO << "UnitMix: unit=" << baseId << " mst=" << baseMstId
             << " totalExp " << baseTotalExp << "+" << gainedExp << "=" << newTotalExp
             << " lv->" << newLevel << "/" << maxLevel;

    // Step 3: UPDATE base unit level/exp (preserve IMP add_* cols).
    co_await theDb()->execSqlCoro(
        "UPDATE user_units SET unit_lv=$1, exp=$2, total_exp=$3"
        " WHERE user_unit_id=$4 AND user_id=$5;",
        newLevel, newExp, newTotalExp, baseId, std::string(kUserId)
    );

    // Step 4: return spheres equipped on the fodder, then DELETE the material
    // units — deleting without the return would destroy the equipped items.
    if (!matIds.empty())
    {
        co_await gme::returnEquippedSpheres(theDb(), identity, matList);
        co_await theDb()->execSqlCoro(
            "DELETE FROM user_units WHERE user_id=$1 AND user_unit_id IN (" + matList + ");",
            std::string(kUserId)
        );
    }

    // Step 5: deduct zel.
    if (zelCost > 0)
    {
        co_await theDb()->execSqlCoro(
            "UPDATE user_info SET zel = MAX(0, zel - $1) WHERE id=$2;",
            zelCost, std::string(kUserId)
        );
    }


    // Build response.
    UnitMixResp resp = {};

    // Reinforcement animation entry.
    {
        UnitReinforceEntry rd = {};
        rd.handle_name    = "DecompDev";
        rd.target_lv      = newLevel;
        rd.unit_mst_id    = baseMstId;
        rd.base_hp        = br["base_hp"].as<int32_t>();
        rd.base_atk       = br["base_atk"].as<int32_t>();
        rd.base_def       = br["base_def"].as<int32_t>();
        rd.base_heal      = br["base_heal"].as<int32_t>();
        rd.add_hp         = br["add_hp"].as<int32_t>();
        rd.add_atk        = br["add_atk"].as<int32_t>();
        rd.add_def        = br["add_def"].as<int32_t>();
        rd.add_heal       = br["add_heal"].as<int32_t>();
        rd.ext_hp         = br["ext_hp"].as<int32_t>();
        rd.ext_atk        = br["ext_atk"].as<int32_t>();
        rd.ext_def        = br["ext_def"].as<int32_t>();
        rd.skill_id       = std::to_string(br["skill_id"].as<int32_t>());
        rd.skill_lv       = br["skill_lv"].as<int32_t>();
        rd.extra_skill_id = std::to_string(br["extra_skill_id"].as<int32_t>());
        rd.extra_skill_lv = br["extra_skill_lv"].as<int32_t>();
        rd.unit_type_id   = br["unit_type_id"].as<int32_t>();
        rd.mission_id     = "";
        resp.reinforce.emplace_back(std::move(rd));
    }

    // Incremental unit cache update.
    {
        UserUnitInfo ud = {};
        ud.user_id            = std::string(kUserId);
        ud.user_unit_id       = br["user_unit_id"].as<int32_t>();
        ud.unit_id            = baseMstIdInt;
        ud.unit_type_id       = br["unit_type_id"].as<int32_t>();
        ud.unit_lvl            = newLevel;
        ud.exp                = newExp;
        ud.total_exp          = newTotalExp;
        ud.base_hp            = br["base_hp"].as<int32_t>();
        ud.add_hp             = br["add_hp"].as<int32_t>();
        ud.ext_hp             = br["ext_hp"].as<int32_t>();
        ud.limit_over_hp      = br["limit_over_hp"].as<int32_t>();
        ud.base_atk           = br["base_atk"].as<int32_t>();
        ud.add_atk            = br["add_atk"].as<int32_t>();
        ud.ext_atk            = br["ext_atk"].as<int32_t>();
        ud.limit_over_atk     = br["limit_over_atk"].as<int32_t>();
        ud.base_def           = br["base_def"].as<int32_t>();
        ud.add_def            = br["add_def"].as<int32_t>();
        ud.ext_def            = br["ext_def"].as<int32_t>();
        ud.limit_over_def     = br["limit_over_def"].as<int32_t>();
        ud.base_rec          = br["base_heal"].as<int32_t>();
        ud.add_rec           = br["add_heal"].as<int32_t>();
        ud.ext_rec           = br["ext_heal"].as<int32_t>();
        ud.limit_over_rec    = br["limit_over_heal"].as<int32_t>();
        ud.element            = br["element"].as<std::string>();
        ud.leader_skill_id    = br["leader_skill_id"].as<int32_t>();
        ud.bb_id           = std::to_string(br["skill_id"].as<int32_t>());
        ud.bb_lvl           = br["skill_lv"].as<int32_t>();
        ud.sbb_id     = std::to_string(br["extra_skill_id"].as<int32_t>());
        ud.sbb_lvl     = br["extra_skill_lv"].as<int32_t>();
        ud.equipitem_id       = br["eqip_item_id"].as<int32_t>();
        ud.equipitem_frame_id = br["eqip_item_frame_id"].as<int32_t>();
        ud.equipitem_id2      = br["eqip_item_id2"].as<int32_t>();
        ud.equipitem_frame_id2= br["eqip_item_frame_id2"].as<int32_t>();
        ud.fe_bp              = br["fe_bp"].as<int32_t>();
        ud.fe_max_usable_bp   = br["fe_max_usable_bp"].as<int32_t>();
        ud.is_new           = true;
        resp.unit_update.emplace_back(std::move(ud));
    }

    resp.team_info = std::move(
        (co_await gme::getTeamInfo(theDb(), identity)).nonEmpty());

    std::string buffer{};
    if (const auto& ec2 = glz::write_json(resp, buffer); ec2)
    {
        LOG_ERROR << "UnitMix: serialization error: " << glz::format_error(ec2, buffer);
        co_return HandleResult::error("Serialization error");
    }

    co_return HandleResult::success(buffer);
}
