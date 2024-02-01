#pragma once

#include <drogon/HttpController.h>
#include "GmeTypes.hpp"
#include "GmeHandler.hpp"

using HandlerCreateFunc = std::function< Handler::IHandler* ()>;
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

	METHOD_LIST_BEGIN
		ADD_METHOD_TO(GmeController::HandleGame, "/bf/gme/action.php", Post);
	METHOD_LIST_END

private:
	void InitializeHandlers();

	inline void InitializeHandler(const std::string& rq, HandlerCreateFunc fnc) { m_handlers.insert_or_assign(rq, fnc); }

	drogon::HttpResponsePtr newGmeOkResponse(const std::string& reqId, const std::string& aesKey, const Json::Value& data);
	drogon::HttpResponsePtr newGmeErrorResponse(const std::string& reqId, ErrorID errId, ErrorOperation errContinueOp, const std::string& msg);

	std::unordered_map<std::string, HandlerCreateFunc> m_handlers;
};