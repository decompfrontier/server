#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/utils/Random.hpp>
#include <gimuserver/packets/net_handlers.hpp>

HANDLEF(Initialize)
{
	UserInfoReq req = {};
	const auto& ec = glz::read_json(req, json);
	if (ec)
	{
		LOG_DEBUG << "Cannot deserialize Initialize: " << ec;
		co_return "";
	}

	InitializeResp resp = theServer()->cache().initializeResp();


	const auto& res = co_await theDb()->execSqlCoro("SELECT account_id, username, admin FROM users WHERE id=`$1`", req.user_id);
	if (res.empty())
	{
		resp.userInfo.handle_name = "Offline";
		resp.userInfo.debug_mode = false;
		resp.userInfo.account_id = RandomId();
		resp.userInfo.tutorial_end_flag = true;
		resp.userInfo.tutorial_status = 0;
		// persist user info
	}
	else
	{
		// only one query pls
		const auto& sql = res[0];
		size_t col = 0;
		resp.userInfo.account_id = sql[col++].as<std::string>();
		resp.userInfo.handle_name = sql[col++].as<std::string>();
		resp.userInfo.debug_mode = sql[col++].as<bool>();
	}

	resp.signalKey.key = "C7vnXA5T";
	resp.challengeArenaUserInfo.unk = "n9ZMPC0t"; // rank name?
	resp.challengeArenaUserInfo.unkstr2 = "F"; // ranking?
	resp.challengeArenaUserInfo.league_id = 1;

	resp.summonerJournal.user_id = resp.userInfo.user_id;

	std::string buffer{};
	const auto& ec2 = glz::write_json(resp, buffer);
	if (ec2)
	{
		LOG_DEBUG << "Cannot serialize Initialize: " << ec2;
		co_return "";
	}

	co_return buffer;
}
