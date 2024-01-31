#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

/**
* Controller for Brave Frontier web services
* @note URL: win.bfww.gumi.sg/bf/web/
*/
class BfWebController : public drogon::HttpController<BfWebController>
{
public:
	void HandleWebPage(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback);
	void HandleDefault(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback);

	METHOD_LIST_BEGIN
		//ADD_METHOD_TO(BfWebController::HandleDefault, "/", Get);
		ADD_METHOD_VIA_REGEX(BfWebController::HandleWebPage, "/bf/web/(.*)", Get);
		ADD_METHOD_VIA_REGEX(BfWebController::HandleWebPage, "/content/(.*)", Get);
	METHOD_LIST_END
};
