#include "App.hpp"
#include "DlsController.hpp"

using namespace drogon;

void DlsController::HandleDls(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
	DumpLog log;
	theServer()->tryOpenHttpDumpLog("DLS", log);
	log << rq << "\n";

	const auto& bodyRsp = theServer()->cache().dls();
	log << bodyRsp;

	auto resp = HttpResponse::newHttpResponse();
	resp->setContentTypeCode(ContentType::CT_APPLICATION_JSON);
	resp->setStatusCode(k200OK);
	resp->setBody(bodyRsp);
	resp->setCloseConnection(true);

	callback(resp);
}
