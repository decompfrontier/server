#pragma once

#include <drogon/HttpController.h>
#include "GmeTypes.hpp"
#include "GmeHandler.hpp"

using namespace drogon;

/**
* Controller for Brave Frontier API
* @note URL: ios21900.bravefrontier.gumi.sg
*/
class GmeController : public drogon::HttpController<GmeController>
{
public:
	GmeController();

	void HandleGame(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback);
	void HandleFeatureCheck(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback);
	void HandleServerTime(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback);

	METHOD_LIST_BEGIN
		ADD_METHOD_TO(GmeController::HandleGame, "/bf/gme/action.php", Post);
		ADD_METHOD_TO(GmeController::HandleFeatureCheck, "/bf/gme/featureCheck.php", Get);
		ADD_METHOD_TO(GmeController::HandleServerTime, "/bf/gme/action/getServerTime.php", Get);
	METHOD_LIST_END

private:
	void InitializeHandlers();

	inline void InitializeHandler(std::shared_ptr<Handler::HandlerBase>&& p) { m_handlers.insert_or_assign(p->GetGroupId(), p); }

	std::unordered_map<std::string, std::shared_ptr<Handler::HandlerBase>> m_handlers;
	std::unordered_map<std::string, UserInfo> m_users;
};
