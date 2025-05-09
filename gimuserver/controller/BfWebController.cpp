#include "App.hpp"
#include "BfWebController.hpp"
#include "WebTerms.hpp"

#include <filesystem>

using namespace drogon;
namespace fs = std::filesystem;

void BfWebController::HandleWebPage(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
	LOG_REQ << rq;

#if 0
	if (rq->getPath() == "/bf/web/terms.htm")
	{
		// TODO: convert this to a drogon view controller
		callback(HttpResponse::newFileResponse((const unsigned char*)WEB_TERMS_DATA, strlen(WEB_TERMS_DATA)));
		return;
	}

	auto path = System::Instance().GetContentRootPath() + rq->getPath();

	if (!fs::exists(path) || fs::is_directory(path))
	{
		Utils::AddMissingDlcFile(rq->getPath());
		callback(HttpResponse::newNotFoundResponse());
		return;
	}

	callback(HttpResponse::newFileResponse(path));
#endif
}

void BfWebController::HandleDefault(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
#if 0
	BfWebController pp;
	LOG_REQ << pp;

	HttpRequestPtr x2;
	LOG_REQ << x2;
	//LOG_REQ << "WebController->HandleDefault" << rq;
	callback(HttpResponse::newHttpResponse());
#endif
}