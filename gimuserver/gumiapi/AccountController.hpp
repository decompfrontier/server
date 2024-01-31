#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

/**
* Controller for Gumi Live API registration
* @note URL: api-sl.gl.gumi.sg
*/
class AccountController : public drogon::HttpController<AccountController>
{
public:
	void HandleGuest(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback);

	METHOD_LIST_BEGIN
		ADD_METHOD_TO(AccountController::HandleGuest, "/accounts/guest/login/", Get);
	METHOD_LIST_END
};
