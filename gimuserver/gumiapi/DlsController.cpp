#include "DlsController.hpp"
#include "core/Utils.hpp"
#include "core/BfCrypt.hpp"
#include "core/Config.hpp"

using namespace drogon;

void DlsController::HandleDls(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
	Utils::DumpInfoToDrogon(rq, "DLS");

	Json::Value json;
	json["game"] = Utils::GetDrogonBindHostname();
	json["resource"] = Utils::GetDrogonBindHostname() + "/";
	json["mstv"] = "21900"; // TODO: make this configurable as well
	json["gumilive"] = Utils::GetDrogonBindHostname() + "/";
	json["bgimage"] = "/" + Config::Instance().GetWallpaperInfo();

	Json::Value output;
	output["SREE"] = BfCrypt::CryptSREE(json);

	callback(HttpResponse::newHttpJsonResponse(output));
}
