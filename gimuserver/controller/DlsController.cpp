#include "App.hpp"
#include "DlsController.hpp"

using namespace drogon;

void DlsController::HandleDls(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
	logReq() << rq;

	auto resp = HttpResponse::newHttpResponse();
	resp->setContentTypeCode(ContentType::CT_APPLICATION_JSON);
	resp->setStatusCode(k200OK);
	resp->setBody(theServer()->cache().dls());
	callback(resp);
}
