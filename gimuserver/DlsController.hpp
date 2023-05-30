#pragma once

#include <drogon/HttpSimpleController.h>

using namespace drogon;

/**
* Controller for Gimu Live API game info
* @note URL: api-sl.bfww.gumi.sg/dls
*/
class DlsController : public drogon::HttpSimpleController<DlsController>
{
public:
	void asyncHandleHttpRequest(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) override;

	PATH_LIST_BEGIN
	PATH_ADD("/dls", Get);
	PATH_LIST_END
};