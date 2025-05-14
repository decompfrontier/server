#pragma once

#include <gimuserver/packets/net_gme.hpp>

/**
* Controller for Brave Frontier API
* <p>URL: ios21900.bravefrontier.gumi.sg
*/
class GmeController : public drogon::HttpController<GmeController>
{
public:
	/*!
	* Main handler of game packets.
	* @param[in] rq HTTP request
	* @param[in] callback Callback to send the response
	* @return Asyncronous task to await
	*/
	drogon::Task<> HandleGame(drogon::HttpRequestPtr rq, std::function<void(const drogon::HttpResponsePtr&)> callback);

	/*!
	* Handles the feature check request.
	* @param[in] rq HTTP request
	* @param[in] callback Callback to send the response
	*/
	void HandleFeatureCheck(const drogon::HttpRequestPtr& rq, std::function<void(const drogon::HttpResponsePtr&)>&& callback);

	/*!
	* Handles the server time request.
	* @param[in] rq HTTP request
	* @param[in] callback Callback to send the response
	*/
	void HandleServerTime(const drogon::HttpRequestPtr& rq, std::function<void(const drogon::HttpResponsePtr&)>&& callback);

	/*!
	* Handles the daily login global configuration.
	* @param[in] rq HTTP request
	* @param[in] callback Callback to send the response
	*/
	void HandleDailyLogin(const drogon::HttpRequestPtr& rq, std::function<void(const drogon::HttpResponsePtr&)>&& callback);

	METHOD_LIST_BEGIN
		ADD_METHOD_TO(GmeController::HandleGame, "/bf/gme/action.php", drogon::Post);
		ADD_METHOD_TO(GmeController::HandleFeatureCheck, "/bf/gme/featureCheck.php", drogon::Get);
		ADD_METHOD_TO(GmeController::HandleServerTime, "/bf/gme/action/getServerTime.php", drogon::Get);
		ADD_METHOD_TO(GmeController::HandleDailyLogin, "/bf/gme/action/Daily_login.php", drogon::Post);
	METHOD_LIST_END

private:
	/*!
	* Handles a gme message
	* @param session Session that requested the message
	* @param gme Input message
	* @return A task which will return the output message
	*/
	drogon::Task<GmeAction> Handle(drogon::SessionPtr session, const GmeAction& gme);
};
