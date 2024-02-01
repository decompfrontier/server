#include "DlsController.hpp"
#include "core/Utils.hpp"
#include "core/BfCrypt.hpp"
#include "core/System.hpp"

using namespace drogon;

void DlsController::HandleDls(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
	const auto& serverInfo = System::Instance().ServerConfig();

	Utils::DumpInfoToDrogon(rq, "DLS");
	
	Json::Value json;
	json["game"] = Utils::GetDrogonBindHostname();
	json["resource"] = Utils::GetDrogonBindHostname();
	json["mstv"] = std::to_string(serverInfo.GameVersion);
	json["gumilive"] = Utils::GetDrogonBindHostname() + "/";
	json["bgimage"] = serverInfo.Wallpaper;

	Json::Value output;
	output["SREE"] = BfCrypt::CryptSREE(json);

	callback(HttpResponse::newHttpJsonResponse(output));
}
