#include "App.hpp"
#include "OfflineModController.hpp"

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
