#include "InitializeHandler.hpp"
#include "core/Utils.hpp"
#include "core/System.hpp"
#include "gme/requests/UserInfo.hpp"
#include "gme/response/UserInfo.hpp"
#include "gme/response/SignalKey.hpp"
#include "gme/response/DailyTaskBonusMst.hpp"
#include "db/DbMacro.hpp"

void Handler::InitializeHandler::Handle(const drogon::SessionPtr& session, DrogonCallback& cb, const Json::Value& req) const
{
	Request::UserInfo inInfo;
	inInfo.Deserialize(req);

	Response::UserInfo info;

	// NOTE: A real server would check for the gumi api key, we just hardcode into this so we can always login
	session->modify<std::string>("session_id", [](std::string& v) { v = "AAAAAAAA"; });

	// TODO: we probably have a lot of stuff missing
	GME_DB->execSqlAsync("SELECT id, account_id, username, admin FROM users WHERE id=$1",
		[this, req, &cb, &session](const drogon::orm::Result& result) {
			Response::UserInfo ti;

			if (result.size() > 0)
			{
				int col = 0;
				auto& sql = result[0];
				ti.userID = sql[col++].as<std::string>();
				ti.accountID = sql[col++].as<std::string>();
				ti.handleName = sql[col++].as<std::string>();
				ti.debugMode = sql[col++].as<bool>() ? 1 : 0;
			}
			else {
				ti.userID = session->get<std::string>("user_id");
				ti.handleName = "----";
				ti.debugMode = false;
				GME_DB->execSqlSync(
					"INSERT INTO users("
					"id, account_id, username, admin"
					")VALUES ("
					"$1, $2, $3, $4"
					");",
					ti.userID, ti.accountID, ti.handleName, ti.debugMode
				);
			}

			Json::Value res;
			ti.Serialize(res);
			System::Instance().MstInfo().GetMstData(res);
			FinishHandling(session, cb, res);
		},
		[this, req, &cb, &session](const drogon::orm::DrogonDbException& e) {
			Utils::SetSessionError(session, ErrorOperation::Close, e.base().what());
			FinishHandling(session, cb);
		},
		session->get<std::string>("user_id")
	);
}
