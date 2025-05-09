#pragma once

/**
* Controller for Gumi Live API registration.
* @note URL: api-sl.gl.gumi.sg
*/
class AccountController : public drogon::HttpController<AccountController>
{
public:
	/*!
	* Handle login request for guest (not facebook) logins.
	* @param[in] rq HTTP request
	* @param[in] callback Callback to continue handling data
	*/
	void HandleGuest(const drogon::HttpRequestPtr& rq, std::function<void(const drogon::HttpResponsePtr&)>&& callback);

	METHOD_LIST_BEGIN
		ADD_METHOD_TO(AccountController::HandleGuest, "/accounts/guest/login/", Get);
	METHOD_LIST_END
};
