#pragma once

/**
* Controller for Brave Frontier web services
* <p>URL: win.bfww.gumi.sg/bf/web/
*/
class BfWebController : public drogon::HttpController<BfWebController>
{
public:
	/*!
	* Handles file download requests
	* @param[in] rq HTTP request
	* @param[in] callback Callback to send the response
	*/
	void HandleWebPage(const drogon::HttpRequestPtr& rq, std::function<void(const drogon::HttpResponsePtr&)>&& callback);

	/*!
	* Handles a default connection to the server
	* @param[in] rq HTTP request
	* @param[in] callback Callback to send the response
	*/
	void HandleDefault(const drogon::HttpRequestPtr& rq, std::function<void(const drogon::HttpResponsePtr&)>&& callback);

	METHOD_LIST_BEGIN
		//ADD_METHOD_TO(BfWebController::HandleDefault, "/", Get);
		ADD_METHOD_VIA_REGEX(BfWebController::HandleWebPage, "/bf/web/(.*)", drogon::Get);
		ADD_METHOD_VIA_REGEX(BfWebController::HandleWebPage, "/content/(.*)", drogon::Get);
		ADD_METHOD_VIA_REGEX(BfWebController::HandleWebPage, "//content/(.*)", drogon::Get);
		ADD_METHOD_VIA_REGEX(BfWebController::HandleWebPage, "/mst/(.*)", drogon::Get);
		ADD_METHOD_VIA_REGEX(BfWebController::HandleWebPage, "//mst/(.*)", drogon::Get);
	METHOD_LIST_END
};
