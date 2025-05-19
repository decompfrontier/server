#include "App.hpp"
#include "Handlers.hpp"

#include <gimuserver/packets/net_userinfo.hpp>

HANDLEF(Initialize)
{
	UserInfoReq req = {};
	const auto& ec = glz::write_json(req, json);
	if (ec)
	{
		co_return "";
	}

	const auto& res = co_await theDb()->execSqlCoro("SELECT account_id, username, admin FROM users WHERE id=$1");
	if (res.empty())
	{

	}




	co_return "";
}
