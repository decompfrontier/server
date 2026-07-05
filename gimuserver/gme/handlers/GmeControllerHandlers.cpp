#include "App.hpp"
#include "Handlers.hpp"
#include <gimuserver/utils/BfCrypt.hpp>

/*!
* Simple structure to hold handler definitions.
*/
struct GmeHandler
{
	/*!
	* Handler name.
	*/
	const char* name;

	/*!
	* JSON AES cryptation key.
	*/
	const char* key;

	/*!
	* Pointer to a function handler.
	*/
	HandlerFunc func;
};

/*!
* Calculate the hash of a string view.
* @param[in] in String to hash
* @return Hashed string
*/
static constexpr auto hash(std::string_view in)
{
	uint64_t hash = 0;
	for (char c : in) {
		hash = (hash * 131) + c;
	}
	return hash;
}

/*!
* Hashing extension for C strings.
* @param[in] str String to hash
* @param[in] len Length of the string
* @return Hashed string
*/
static constexpr auto operator"" _hash(const char* str, size_t len)
{
	return hash(std::string_view(str, len));
}

/*!
* Registers a new handler.
* @param[in] id Handler ID
* @param[in] func Handler function
* @param[in] key Handler AES key
*/
#define REGISTER(id, func, key) case id##_hash: return { #func, key, GmeHandlers::func }

/*!
* Gets the handler of a message.
* @param cmd Message ID
* @return Output handler
*/
static GmeHandler getHandler(std::string_view cmd)
{
	switch (hash(cmd))
	{
	default:
		return { nullptr, nullptr, nullptr };


	REGISTER("MfZyu1q9", Initialize, "EmcshnQoDr20TZz1");
	REGISTER("Zw3WIoWu", ChallengeArenaResetInfo, "KlwYMGF1");
	REGISTER("nJ3A7qFp", BadgeInfo, "bGxX67KB");
	REGISTER("uYF93Mhc", ControlCenterEnter, "d0k6LGUu");
	REGISTER("m2Ve9PkJ", DeckEdit, "d7UuQsq8");
	REGISTER("2o4axPIC", FriendGet, "EoYuZ2nbImhCU1c0");
	REGISTER("F7JvPk5H", GachaAction, "bL9fipzaSy7xN2w1");
	REGISTER("Uo86DcRh", GachaList, "8JbxFvuSaB2CK7Ln");
	REGISTER("k57TdKDj", UnitSelectorGachaTicket, "1IJ8SaNk");
	REGISTER("NiYWKdzs", HomeInfo, "f6uOewOD");
	REGISTER("9TvyNR5H", MissionEnd, "oINq0rfUFPx5MgmT");
	REGISTER("jE6Sp0q4", MissionStart, "csiVLDKkxEwBfR70");
	REGISTER("TA4MnZX8", NgwordCheck, "r4Smw5TX");
	REGISTER("uV6yH5MX", CreateUser, "4agnATy2DrJsWzQk");
	REGISTER("d36DaiJl", TutorialSkip, "p3qD61db");
	REGISTER("T1nCVvx4", TutorialUpdate, "7hqzmR3T");
	REGISTER("ynB7X5P9", UpdateInfoLight, "7kH9NXwC");
	REGISTER("cTZ3W2JG", UserInfo, "ScJx6ywWEb0A3njT");
	REGISTER("2p9LHCNh", UnitFavorite,            "cb4ESLa1");
	REGISTER("0gUSE84e", UnitEvo,                 "biHf01DxcrPou5Qt");
	REGISTER("Mw08CIg2", UnitMix,                 "JnegC7RrN3FoW8dQ");
	REGISTER("Ri3uTq9b", UnitSell,                "92VqcGFWuPkmT60U");
	REGISTER("CuQ5oB8U", TownUpdate,              "w1eo2ZDJ");
	REGISTER("8v43tz7g", TownFacilityUpdate,       "rq7Yd1nG");
	REGISTER("f49als4D", EventTokenInfo,           "94lDsgh4");

	// Quest / world-map entry point.
	REGISTER("Zds63G5y", AreaInfo,             "YfAh7gqojdXEtFR1");

	// Campaign subsystem (see HANDLER_BLUEPRINT.md §7).
	REGISTER("6Y0gaPQN", CampaignStart,        "WM6yr4ej");
	REGISTER("RSm6p2d4", CampaignMissionGet,   "5jzXN7AH");
	REGISTER("C3a0VnQK", CampaignDeckGet,      "q2ZtYJ6P");
	REGISTER("h1RjcD3S", CampaignBattleStart,  "4CKoVAq0");
	REGISTER("pTNB6yw3", CampaignBattleEnd,    "t06HFsXP");
	REGISTER("5Imq3wC0", CampaignReceipt,      "4DAgP80B");
	REGISTER("jF9Kkro4", CampaignEnd,          "4X9tBSg8");

	REGISTER("gLRIn74v", FixGiftInfo,          "15gTE9ft");

	// World-map / Grand Gaia entry sequence stubs.
	REGISTER("BjAt1D6b", DungeonEventUpdate,     "k5EiNe9x");
	REGISTER("VRfsv4e3", GetScenarioPlayingInfo, "Bh4WqR01");
	REGISTER("1MJT6L3W", UpdatePermitPlaceInfo,  "3zip5Htw");
	REGISTER("rCB7ZI8x", UpdateEventInfo,        "L1o4eGbi");
	REGISTER("5o8ZlDGX", Chronology,             "SNrhAG29");

	}
}

drogon::Task<GmeAction> GmeController::Handle(drogon::SessionPtr session, const GmeAction& gme)
{
	const auto& body = gme.body.value();
	const auto& header = gme.header;
	const auto& handler = getHandler(header.id);

	GmeAction resp{};
	resp.header = gme.header;
	resp.header.client_id = "---";

	if (!handler.func || !handler.key)
	{
		GmeError err{};
		err.cmd = GmeErrorCommand::Close;
		err.flag = GmeErrorFlags::IsInError;
		err.message = std::format("Unsupported request: {}", header.id);
		resp.error = err;
	}
	else
	{
		const auto& decryptedGme = BfCrypt::ReadGME(gme, handler.key);
		if (!decryptedGme.has_value())
		{
			//LOG_ERROR << "Cannot read GME for " << handler.key;
			GmeError err{};
			err.cmd = GmeErrorCommand::Close;
			err.flag = GmeErrorFlags::IsInError;
			err.message = std::format("Unable to decode request: {}", header.id);
			resp.error = err;
		}
		else
		{
			const auto& inputJson = decryptedGme.value();
			DumpLog logReq;
			theServer()->tryOpenHttpDumpLog(header.id, logReq);

			logReq << "REQUEST: " << inputJson << "\n";

			try
			{
				const auto& outputJson = co_await handler.func(session, inputJson);

				if (outputJson.isError())
				{
					logReq << "RESPONSE IN ERROR: " << outputJson.errorMsg << " ex: " << outputJson.exceptionMsg << "\n";
					GmeError err{};
					err.cmd = GmeErrorCommand::Close;
					err.flag = GmeErrorFlags::IsInError;
					err.message = std::format("Unable to handle request: \"{}\", Error: \"{}\"", header.id, outputJson.errorMsg);
					resp.error = err;
				}
				else
				{
					logReq << "RESPONSE: " << outputJson.successJson << "\n";
					resp.body = BfCrypt::BuildGME(outputJson.successJson, handler.key);
				}
			}
			catch (const drogon::orm::DrogonDbException& ex)
			{
				LOG_ERROR << "Handler error " << header.id << " (" << handler.name << ") database exception: " << ex.base().what();
				logReq << "EXCEPTION (db): " << ex.base().what() << "\n";
				GmeError err{};
				err.cmd = GmeErrorCommand::Close;
				err.flag = GmeErrorFlags::IsInError;
				err.message = std::format("Unable to run database query: \"{}\"", header.id);
				resp.error = err;
			}
			catch (const std::exception& ex)
			{
				LOG_ERROR << "Handler error " << header.id << " (" << handler.name << ") exception: " << ex.what();
				logReq << "EXCEPTION (std): " << ex.what() << "\n";
				GmeError err{};
				err.cmd = GmeErrorCommand::Close;
				err.flag = GmeErrorFlags::IsInError;
				err.message = std::format("Unable to handle request: \"{}\", Error: \"{}\"", header.id, ex.what());
				resp.error = err;
			}
		}
	}

	co_return resp;
}
