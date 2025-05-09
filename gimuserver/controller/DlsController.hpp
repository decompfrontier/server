#pragma once

/*!
* Controller for Gumi Live API game info
* @note URL: api-sl.bfww.gumi.sg
*/
class DlsController : public drogon::HttpController<DlsController>
{
public:
	/*!
	* Handle the dls request.
	* @param[in] rq HTTP request
	* @param[in] callback Callback to continue handling data
	*/
	void HandleDls(const drogon::HttpRequestPtr& rq, std::function<void(const drogon::HttpResponsePtr&)>&& callback);

	METHOD_LIST_BEGIN
		ADD_METHOD_TO(DlsController::HandleDls, "/dls", Get);
	METHOD_LIST_END
};
