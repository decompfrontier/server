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
	void HandleGame(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback);

	METHOD_LIST_BEGIN
		ADD_METHOD_TO(GmeController::HandleGame, "/bf/gme/action.php", Post);
	METHOD_LIST_END

private:
	drogon::HttpResponsePtr newGmeOkResponse(const std::string& reqId, const Json::Value& data);
	drogon::HttpResponsePtr newGmeErrorResponse(const std::string& reqId, ErrorID errId, ErrorOperation errContinueOp, const std::string& msg);

	GmeHandler m_handler;
};
