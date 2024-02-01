#include "AccountController.hpp"
#include "core/Utils.hpp"

void AccountController::HandleGuest(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
	Utils::DumpInfoToDrogon(rq, "AccountController");

	Json::Value v;
	v["status"] = "successful"; // "error" if in error
	v["token"] = "test_token"; // TODO: use sqlite as database
	v["game_user_id"] = "5484848548548"; // TODO: use sqlite as database
	v["status_no"] = "0";
	callback(HttpResponse::newHttpJsonResponse(v));
}
