#include "BfWebController.hpp"
#include "core/Utils.hpp"
#include "core/Config.hpp"
#include "WebTerms.hpp"

#include <filesystem>

using namespace drogon;
namespace fs = std::filesystem;

void BfWebController::HandleWebPage(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
	Utils::DumpInfoToDrogon(rq, "WebController");

	if (rq->getPath() == "/bf/web/terms.htm")
	{
		// TODO: convert this to a drogon view controller
		callback(HttpResponse::newFileResponse((const unsigned char*)WEB_TERMS_DATA, strlen(WEB_TERMS_DATA)));
		return;
	}

	auto path = Config::Instance().GetContentRootPath() + rq->getPath(); // TODO: this will be BROKEN for offline mods!

	if (!fs::exists(path) || fs::is_directory(path))
	{
		callback(HttpResponse::newNotFoundResponse());
		return;
	}

	callback(HttpResponse::newFileResponse(path));
}

void BfWebController::HandleDefault(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
	Utils::DumpInfoToDrogon(rq, "WebController");
	callback(HttpResponse::newHttpResponse());
}
