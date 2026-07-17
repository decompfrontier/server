#include "App.hpp"
#include "OfflineModController.hpp"

#include <gimuserver/archive/MissionArchiver.hpp>

using namespace drogon;

void OfflineModController::HandleFpsCap(const HttpRequestPtr& rq,
	std::function<void(const HttpResponsePtr&)>&& callback)
{
	(void)rq;
	const auto cap = theServer()->cache().serverConfig().fpsCap;

	auto resp = HttpResponse::newHttpResponse();
	resp->setContentTypeCode(ContentType::CT_TEXT_PLAIN);
	resp->setStatusCode(k200OK);
	resp->setBody(std::to_string(cap));
	resp->setCloseConnection(true);

	callback(resp);
}

void OfflineModController::HandleReloadMissions(const HttpRequestPtr& rq,
	std::function<void(const HttpResponsePtr&)>&& callback)
{
	(void)rq;
	const auto result = MissionArchiver::instance().reload();

	Json::Value body;
	body["ok"] = result.ok;
	body["missions"] = static_cast<Json::UInt64>(result.missions);
	body["ais"] = static_cast<Json::UInt64>(result.ais);
	if (!result.ok)
	{
		body["error"] = result.error;
	}

	auto resp = HttpResponse::newHttpJsonResponse(body);
	resp->setStatusCode(result.ok ? k200OK : k500InternalServerError);
	resp->setCloseConnection(true);
	callback(resp);
}
