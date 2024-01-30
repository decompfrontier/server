#include "DlsController.hpp"
#include "Utils.hpp"
#include "BfCrypt.hpp"

using namespace drogon;

void DlsController::HandleDls(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
	Json::Value json;
	json["game"] = "Brave Frontier";
	json["resource"] = "http://127.0.0.1:9960/bf/data/";
	json["mstv"] = 1163;
	json["gumilive"] = "http://127.0.0.1:9960/";
	json["force"] = true;
	json["force_msg"] = "Offline Mod coming soon!";
	json["bgimage"] = "wallpapers/test.png";

	//Utils::DumpInfoToDrogon(rq, "DLS");

	Json::Value output;
	output["SREE"] = BfCrypt::CryptSREE(json);

	callback(HttpResponse::newHttpJsonResponse(output));
}
