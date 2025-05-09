#pragma once

/**
* Controller for Brave Frontier web services
* @note URL: win.bfww.gumi.sg/bf/web/
*/
class BfWebController : public drogon::HttpController<BfWebController>
{
public:
	void HandleWebPage(const drogon::HttpRequestPtr& rq, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
	void HandleDefault(const drogon::HttpRequestPtr& rq, std::function<void(const drogon::HttpResponsePtr&)>&& callback);

	METHOD_LIST_BEGIN
		//ADD_METHOD_TO(BfWebController::HandleDefault, "/", Get);
		ADD_METHOD_VIA_REGEX(BfWebController::HandleWebPage, "/bf/web/(.*)", Get);
		ADD_METHOD_VIA_REGEX(BfWebController::HandleWebPage, "/content/(.*)", Get);
		ADD_METHOD_VIA_REGEX(BfWebController::HandleWebPage, "//content/(.*)", Get);
		ADD_METHOD_VIA_REGEX(BfWebController::HandleWebPage, "/mst/(.*)", Get);
		ADD_METHOD_VIA_REGEX(BfWebController::HandleWebPage, "//mst/(.*)", Get);
	METHOD_LIST_END
};
