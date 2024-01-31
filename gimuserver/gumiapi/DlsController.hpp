#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

/**
* Controller for Gumi Live API game info
* @note URL: api-sl.bfww.gumi.sg
*/
class DlsController : public drogon::HttpController<DlsController>
{
public:
	void HandleDls(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback);

	METHOD_LIST_BEGIN
		ADD_METHOD_TO(DlsController::HandleDls, "/dls", Get);
	METHOD_LIST_END
};
