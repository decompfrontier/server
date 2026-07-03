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
	REGISTER("NiYWKdzs", HomeInfo, "f6uOewOD");
	REGISTER("9TvyNR5H", MissionEnd, "oINq0rfUFPx5MgmT");
	REGISTER("jE6Sp0q4", MissionStart, "csiVLDKkxEwBfR70");
	REGISTER("TA4MnZX8", NgwordCheck, "r4Smw5TX");
	REGISTER("uV6yH5MX", CreateUser, "4agnATy2DrJsWzQk");
	REGISTER("d36DaiJl", TutorialSkip, "p3qD61db");
	REGISTER("T1nCVvx4", TutorialUpdate, "7hqzmR3T");
	REGISTER("ynB7X5P9", UpdateInfoLight, "7kH9NXwC");
	REGISTER("cTZ3W2JG", UserInfo, "ScJx6ywWEb0A3njT");

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
				GmeError err{};
				err.cmd = GmeErrorCommand::Close;
				err.flag = GmeErrorFlags::IsInError;
				err.message = std::format("Unable to run database query: \"{}\"", header.id);
				resp.error = err;
			}
			catch (const std::exception& ex)
			{
				LOG_ERROR << "Handler error " << header.id << " (" << handler.name << ") exception: " << ex.what();
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
