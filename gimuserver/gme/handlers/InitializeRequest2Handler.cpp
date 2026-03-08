#include "InitializeRequest2Handler.hpp"
#include "core/Utils.hpp"
#include "core/System.hpp"
#include "gme/requests/UserInfo.hpp"
#include "gme/response/UserInfo.hpp"
// DEV_SKIP_TUTORIAL flag — defined in AccountController.hpp/cpp.
// We need it here to gate the tutorialEndFlag force-set in OnUserInfoSuccess.
// TODO (tutorial): Remove this include when DEV_SKIP_TUTORIAL is retired.
#ifndef DEV_SKIP_TUTORIAL
#define DEV_SKIP_TUTORIAL 0
#endif
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

void Handler::InitializeRequest2Handler::Handle(UserInfo& user, DrogonCallback cb, const Json::Value& req) const
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


void Handler::InitializeRequest2Handler::OnUserInfoSuccess(const drogon::orm::Result& result, DrogonCallback cb, UserInfo& user) const
{
	if (result.size() > 0)
	{
		int col = 0;
		auto& sql = result[0];
		user.info.accountID = sql[col++].as<std::string>();
		user.info.handleName = sql[col++].as<std::string>();
		user.info.debugMode = sql[col++].as<bool>() ? 1 : 0;

		// DEV_SKIP_TUTORIAL: Force tutorial complete for all existing users.
		//
		// Root cause of the post-asset-validation crash:
		//   AccountController::HandleGuest inserts the user row into the DB before
		//   this handler is ever called, so result.size() > 0 is ALWAYS true for our
		//   seeded account. The else-branch below (which sets tutorialEndFlag=1) is
		//   therefore never reached. tutorialEndFlag stays at the struct default of 0,
		//   the client receives "tutorial not complete", attempts the tutorial sequence,
		//   hits an unimplemented tutorial handler, gets a bad/empty response, crashes.
		//
		// TODO (tutorial): Remove this block once tutorialEndFlag is persisted in the
		//   users table. Add a tutorial_end_flag INTEGER column via a new migration:
		//     "ALTER TABLE users ADD COLUMN tutorial_end_flag INTEGER DEFAULT 0;"
		//   The tutorial-completion handler should UPDATE it to 1 after the player
		//   picks their starter unit. Read it back here alongside account_id/username.
#if DEV_SKIP_TUTORIAL
		user.info.tutorialEndFlag = 1;
		user.info.tutorialStatus = 0;
#endif
	}
	else {
		// New user that somehow isn't in the DB yet (race between HandleGuest's async
		// INSERT and this handler being called -- should be rare but possible).
		user.info.handleName = "BFOM: 03/03/2024";
		user.info.debugMode = false;
		user.info.accountID = Utils::RandomAccountID();
		user.info.tutorialEndFlag = 1;
		user.info.tutorialStatus = 0;
		//user.PersistInfo();
	}

	Json::Value res;
	user.info.Serialize(res); // user info

	const auto& msts = System::Instance().MstConfig();

	{
		Response::SignalKey k;
		k.key = "C7vnXA5T";
		k.Serialize(res);
	}

	{
		Response::ChallengeArenaUserInfo ci;
		ci.unkstr = "n9ZMPC0t"; // rank name?
		ci.unkstr2 = "F"; // ranking?
		ci.leagueId = 1;
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
		Response::GuildInfo guild;
		guild.Serialize(res);
	}

	{
		Response::DailyTaskMst dailyTasks;

		// TODO: we need a daily task table so that we
		//  can select how many daily tasks we can do
		Response::DailyTaskMst::Data d;
		d.typeKey = "AV";
		d.typeTitle = "Arena Victory";
		d.typeDescription = "Achieve 3 Victories in The Arena ";
		d.taskCount = 3;
		d.taskBravePoints = 20;
		dailyTasks.Mst.emplace_back(d);

		d.typeKey = "VV";
		d.typeTitle = "Vortex Venturer";
		d.typeDescription = "Complete 1 Mission within the Vortex ";
		d.taskCount = 1;
		d.taskBravePoints = 20;
		dailyTasks.Mst.emplace_back(d);

		d.typeKey = "CM";
		d.typeTitle = "Craftsman";
		d.typeDescription = "Craft 5 Items/Spheres ";
		d.taskCount = 5;
		d.taskBravePoints = 20;
		dailyTasks.Mst.emplace_back(d);


		dailyTasks.Serialize(res); // daily taks
	}

	{
		Response::DailyLoginRewardsUserInfo dailyWheel;
		// TODO: make this configurable
		dailyWheel.Serialize(res);
	}

	{
		Response::VideoAdsSlotgameInfo v = msts.GetAdsSlotInfo();
		v.Serialize(res);
	}

	{
		// SummonerJournal
		Response::SummonerJournalUserInfo v;
		v.userId = user.info.userID;
		v.Serialize(res);
	}

	msts.CopyInitializeMstTo(res);

	cb(newGmeOkResponse(GetGroupId(), GetAesKey(), res));
}