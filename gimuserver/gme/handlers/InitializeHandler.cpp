#include "InitializeHandler.hpp"
#include "core/Utils.hpp"
#include "core/System.hpp"
#include "gme/response/UserInfo.hpp"
#include "gme/response/SignalKey.hpp"
#include "gme/requests/UserInfo.hpp"
#include "db/DbMacro.hpp"

void Handler::InitializeHandler::Handle(const Json::Value& req)
{
	Request::UserInfo inInfo;
	inInfo.Deserialize(req);

	Response::UserInfo info;

	GME_DB->execSqlAsync("SELECT id, account_id, username, admin FROM users",
		[this](const drogon::orm::Result& result) {
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
				ti.userID = Utils::RandomUserID();
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
			FinishHandling(res);
		},
		[this](const drogon::orm::DrogonDbException& e) {
			m_errMsg = e.base().what();
			m_errID = ErrorID::Yes;
			FinishHandling();
		}
	);
}

