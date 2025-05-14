#pragma once

/*!
* Controller for Gumi Live API game info
* <p>URL: api-sl.bfww.gumi.sg
*/
class DlsController : public drogon::HttpController<DlsController>
{
public:
	/*!
	* Handle the banner request.
	* @param[in] rq HTTP request
	* @param[in] callback Callback to send the response
	*/
	void HandleDls(const drogon::HttpRequestPtr& rq, std::function<void(const drogon::HttpResponsePtr&)>&& callback);

	METHOD_LIST_BEGIN
		ADD_METHOD_TO(DlsController::HandleDls, "/dls", drogon::Get);
	METHOD_LIST_END
};
