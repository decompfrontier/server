#include "InitializeHandler.hpp"
#include "core/Utils.hpp"
#include "core/System.hpp"
#include "gme/requests/UserInfo.hpp"
#include "gme/response/UserInfo.hpp"
#include "gme/response/SignalKey.hpp"
#include "gme/response/ChallengeArenaUserInfo.hpp"
#include "gme/response/DailyTaskPrizeMst.hpp"
#include "gme/response/DailyTaskBonusMst.hpp"
#include "gme/response/NoticeInfo.hpp"
#include "gme/response/GuildInfo.hpp"
#include "gme/response/DailyTaskMst.hpp"
#include "gme/response/DailyLoginRewardsUserInfo.hpp"
#include "gme/response/BannerInfoMst.hpp"
#include "gme/response/NoticeListMst.hpp"
#include "gme/response/VideoAdsSlotgameInfo.hpp"
#include "gme/response/NpcMst.hpp"
#include "gme/response/SummonerJournalUserInfo.hpp"
#include "db/DbMacro.hpp"

void Handler::InitializeHandler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
{
	Request::UserInfo inInfo;
	inInfo.Deserialize(req);

	// TODO: we probably have a lot of stuff missing
	GME_DB->execSqlAsync("SELECT account_id, username, admin FROM users WHERE id=$1",
		[this, cb, &user](const drogon::orm::Result& res) { OnUserInfoSuccess(res, cb, user); },
		[this, cb](const drogon::orm::DrogonDbException& e) { OnError(e, cb); },
		user.info.userID
	);
}

void Handler::InitializeHandler::OnUserInfoSuccess(const drogon::orm::Result& result, DrogonCallback cb, UserInfo& user) const
{
	if (result.size() > 0)
	{
		int col = 0;
		auto& sql = result[0];
		user.info.accountID = sql[col++].as<std::string>();
		user.info.handleName = sql[col++].as<std::string>();
		user.info.debugMode = sql[col++].as<bool>() ? 1 : 0;
	}
	else {
		user.info.handleName = "----";
		user.info.debugMode = false;
		user.info.accountID = Utils::RandomAccountID();
		//user.PersistInfo();
	}

	Json::Value res;
	user.info.Serialize(res); // user info

	const auto& msts = System::Instance().MstConfig();

	msts.CopyTo(res);

	{
		Response::ChallengeArenaUserInfo ci;
		ci.unkstr = "n9ZMPC0t"; // rank name?
		ci.unkstr2 = "F"; // ranking?
		ci.unkint6 = 1; // ??
		ci.Serialize(res); // challenge arena info
	}

	{
		Response::DailyTaskBonusMst dailytaskbonus;
		for (const auto& v : msts.DailyTaskConfig().GetTaskBonusTables())
		{
			Response::DailyTaskBonusMst::Data d;
			d.bonusBravePoints = v.points;
			dailytaskbonus.Mst.emplace_back(d);
		}
		dailytaskbonus.Serialize(res); // daily task bonus mst
	}

	{
		Response::DailyTaskPrizeMst dailytaskprizes;
		for (const auto& v : msts.DailyTaskConfig().GetTaskPrizes())
		{
			Response::DailyTaskPrizeMst::Data d;
			d.taskId = v.id;
			d.taskTitle = v.title;
			d.taskDesc = v.desc;
			d.bravePointCost = v.points_cost;
			d.currentClaimCount = 0; // TODO: add support of this inside the user info
			d.isMileStonePrize = v.milestone_prize;
			d.maxClaimCount = v.max_claims;
			d.presentType = v.present_type;
			d.targetCount = v.target_count;
			d.targetId = v.target_id;
			d.targetParam = v.target_param;
			d.timeLimit = v.time_limit;
			dailytaskprizes.Mst.emplace_back(d);
		}
		dailytaskprizes.Serialize(res); // daily task prizes mst
	}

	{
		Response::NoticeInfo notices;
		notices.url = "http://ios21900.bfww.gumi.sg/pages/versioninfo";
		notices.Serialize(res);
	}

	{
		Response::GuildInfo guild;
		guild.Serialize(res);
	}

	{
		Response::DailyTaskMst dailyTasks;
		for (int i = 0; i < 3; i++)
		{
			// TODO: we need a daily task table so that we
			//  can select how many daily tasks we can do
			Response::DailyTaskMst::Data d;
			d.typeKey = "AV";
			d.typeTitle = "Arena Victory";
			d.typeDescription = "Achieve 3 Victories in The Arena ";
			d.taskCount = 3;
			d.taskBravePoints = 20;
			d.timesCompleted = 1;
			dailyTasks.Mst.emplace_back(d);
		}
		dailyTasks.Serialize(res); // daily taks
	}

	{
		Response::DailyLoginRewardsUserInfo dailyWheel;
		// TODO: make this configurable
		dailyWheel.Serialize(res);
	}

	{
		Response::BannerInfoMst bannerInfo;
		Response::BannerInfoMst::Data dd;
		dd.name = "Control panel";
		dd.targetOS = "1,2,3,4,5,6,7,8";
		dd.url = "http://ios21900.bfww.gumi.sg/pages/control_panel";
		dd.image = "banner_facebookV2.png";
		bannerInfo.Mst.push_back(dd);
		dd.name = "Wikia";
		dd.targetOS = "1,2,3,4,5,6,7,8";
		dd.url = "http://goo.gl/BUfhDN";
		dd.image = "banner_wikiV2.png";
		bannerInfo.Mst.push_back(dd);
		bannerInfo.Serialize(res);
	}

	{
		Response::VideoAdsSlotgameInfo v;
		v.Serialize(res);
	}

	{
		// SummonerJournal
		Response::SummonerJournalUserInfo v;
		v.userId = user.info.userID;
		v.Serialize(res);
	}

	{
		// UnitExpMst
	}

	{
		// ExtraPassiveSkillMst
	}

	{
		// DungeonKeyMst
	}

	{
		Response::NpcMst mst;
		{ 
			Response::NpcMst::Data d;
			// NPC_1
			d.id = 1;
			d.handle_name = "MST_NPC_1_NAME";
			d.arena_rank_id = 23;
			d.team_info = "{\"P_USER_ID\":\"AAAAAAAA\",\"P_FRIEND_MESSAGE\":\"\",\"P_LV\":\"70\"}";
			d.party_info = "[{\"P_USER_UNIT_ID\":\"71393\",\"P_MEMBER_TYPE\":\"1\",\"P_DISPORDER\":\"2\"},{\"P_USER_UNIT_ID\":\"71408\",\"P_MEMBER_TYPE\":\"0\",\"P_DISPORDER\":\"0\"},{\"P_USER_UNIT_ID\":\"71419\",\"P_MEMBER_TYPE\":\"1\",\"P_DISPORDER\":\"4\"},{\"P_USER_UNIT_ID\":\"71467\",\"P_MEMBER_TYPE\":\"1\",\"P_DISPORDER\":\"3\"},{\"P_USER_UNIT_ID\":\"71796\",\"P_MEMBER_TYPE\":\"1\",\"P_DISPORDER\":\"1\"}]";
			d.unit_info = "[{\"P_USER_UNIT_ID\":\"71393\",\"P_UNIT_ID\":\"20014\",\"P_UNIT_TYPE_ID\":\"5\",\"P_LV\":\"12\",\"P_HP\":\"3815\",\"P_ATK\":\"1355\",\"P_DEF\":\"1127\",\"P_HEL\":\"1461\",\"P_SKILL_ID\":\"20014\",\"P_SKILL_LV\":\"1\",\"P_EQP_ITEM_ID\":\"0\"},{\"P_USER_UNIT_ID\":\"71408\",\"P_UNIT_ID\":\"10114\",\"P_UNIT_TYPE_ID\":\"5\",\"P_LV\":\"12\",\"P_HP\":\"4995\",\"P_ATK\":\"1125\",\"P_DEF\":\"1517\",\"P_HEL\":\"951\",\"P_SKILL_ID\":\"10114\",\"P_SKILL_LV\":\"1\",\"P_EQP_ITEM_ID\":\"0\"},{\"P_USER_UNIT_ID\":\"71419\",\"P_UNIT_ID\":\"60114\",\"P_UNIT_TYPE_ID\":\"5\",\"P_LV\":\"12\",\"P_HP\":\"3986\",\"P_ATK\":\"1503\",\"P_DEF\":\"1171\",\"P_HEL\":\"1898\",\"P_SKILL_ID\":\"60114\",\"P_SKILL_LV\":\"1\",\"P_EQP_ITEM_ID\":\"0\"},{\"P_USER_UNIT_ID\":\"71467\",\"P_UNIT_ID\":\"50114\",\"P_UNIT_TYPE_ID\":\"5\",\"P_LV\":\"12\",\"P_HP\":\"5239\",\"P_ATK\":\"1240\",\"P_DEF\":\"1368\",\"P_HEL\":\"847\",\"P_SKILL_ID\":\"50114\",\"P_SKILL_LV\":\"1\",\"P_EQP_ITEM_ID\":\"0\"},{\"P_USER_UNIT_ID\":\"71796\",\"P_UNIT_ID\":\"60014\",\"P_UNIT_TYPE_ID\":\"5\",\"P_LV\":\"12\",\"P_HP\":\"4876\",\"P_ATK\":\"1198\",\"P_DEF\":\"1400\",\"P_HEL\":\"755\",\"P_SKILL_ID\":\"60014\",\"P_SKILL_LV\":\"1\",\"P_EQP_ITEM_ID\":\"0\"}]";
			mst.Mst.push_back(d);
			// NPC_2
			d.id = 2;
			d.handle_name = "MST_NPC_2_NAME";
			d.arena_rank_id = 12;
			d.team_info = "{\"P_USER_ID\":\"BBBBBBBB\",\"P_FRIEND_MESSAGE\":\"\",\"P_LV\":\"45\"}";
			d.party_info = "[{\"P_USER_UNIT_ID\":\"71393\",\"P_MEMBER_TYPE\":\"1\",\"P_DISPORDER\":\"2\"},{\"P_USER_UNIT_ID\":\"71408\",\"P_MEMBER_TYPE\":\"0\",\"P_DISPORDER\":\"0\"},{\"P_USER_UNIT_ID\":\"71419\",\"P_MEMBER_TYPE\":\"1\",\"P_DISPORDER\":\"4\"},{\"P_USER_UNIT_ID\":\"71467\",\"P_MEMBER_TYPE\":\"1\",\"P_DISPORDER\":\"3\"},{\"P_USER_UNIT_ID\":\"71796\",\"P_MEMBER_TYPE\":\"1\",\"P_DISPORDER\":\"1\"}]";
			d.unit_info = "[{\"P_USER_UNIT_ID\":\"71393\",\"P_UNIT_ID\":\"30023\",\"P_UNIT_TYPE_ID\":\"5\",\"P_LV\":\"12\",\"P_HP\":\"2949\",\"P_ATK\":\"563\",\"P_DEF\":\"699\",\"P_HEL\":\"1512\",\"P_SKILL_ID\":\"\",\"P_SKILL_LV\":\"1\",\"P_EQP_ITEM_ID\":\"0\"},{\"P_USER_UNIT_ID\":\"71408\",\"P_UNIT_ID\":\"50083\",\"P_UNIT_TYPE_ID\":\"5\",\"P_LV\":\"12\",\"P_HP\":\"2945\",\"P_ATK\":\"802\",\"P_DEF\":\"1015\",\"P_HEL\":\"750\",\"P_SKILL_ID\":\"\",\"P_SKILL_LV\":\"1\",\"P_EQP_ITEM_ID\":\"0\"},{\"P_USER_UNIT_ID\":\"71419\",\"P_UNIT_ID\":\"10023\",\"P_UNIT_TYPE_ID\":\"5\",\"P_LV\":\"12\",\"P_HP\":\"2708\",\"P_ATK\":\"845\",\"P_DEF\":\"558\",\"P_HEL\":\"677\",\"P_SKILL_ID\":\"\",\"P_SKILL_LV\":\"1\",\"P_EQP_ITEM_ID\":\"0\"},{\"P_USER_UNIT_ID\":\"71467\",\"P_UNIT_ID\":\"40072\",\"P_UNIT_TYPE_ID\":\"5\",\"P_LV\":\"12\",\"P_HP\":\"1916\",\"P_ATK\":\"579\",\"P_DEF\":\"455\",\"P_HEL\":\"396\",\"P_SKILL_ID\":\"\",\"P_SKILL_LV\":\"1\",\"P_EQP_ITEM_ID\":\"0\"},{\"P_USER_UNIT_ID\":\"71796\",\"P_UNIT_ID\":\"60093\",\"P_UNIT_TYPE_ID\":\"5\",\"P_LV\":\"12\",\"P_HP\":\"2117\",\"P_ATK\":\"882\",\"P_DEF\":\"650\",\"P_HEL\":\"1350\",\"P_SKILL_ID\":\"\",\"P_SKILL_LV\":\"1\",\"P_EQP_ITEM_ID\":\"0\"}]";
			mst.Mst.push_back(d);
		}
		mst.Serialize(res);
	}

	cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
}
