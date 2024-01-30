#include "BfWebController.hpp"
#include "Utils.hpp"

using namespace drogon;

void BfWebController::HandleWebPage(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
	Utils::DumpInfoToDrogon(rq, "WebController");
	callback(HttpResponse::newHttpResponse());
}
