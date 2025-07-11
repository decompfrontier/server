#include "App.hpp"
#include "Handlers.hpp"
#include <gimuserver/utils/BfCrypt.hpp>

/*!
* Simple structure to hold handler definitions.
*/
struct GmeHandler
{
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
#define REGISTER(id, func, key) case id##_hash: return { key, GmeHandlers::func }

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
		return { nullptr, nullptr };


	REGISTER("MfZyu1q9", Initialize, "EmcshnQoDr20TZz1");
	REGISTER("nJ3A7qFp", BadgeInfo, "bGxX67KB");
	REGISTER("uYF93Mhc", ControlCenterEnter, "d0k6LGUu");
	REGISTER("m2Ve9PkJ", DeckEdit, "d7UuQsq8");
	REGISTER("2o4axPIC", FriendGet, "EoYuZ2nbImhCU1c0");
	REGISTER("Uo86DcRh", GatchaList, "8JbxFvuSaB2CK7Ln");
	REGISTER("NiYWKdzs", HomeInfo, "f6uOewOD");
	REGISTER("jE6Sp0q4", MissionStart, "csiVLDKkxEwBfR70");
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
		err.message = std::format("Unsupported request! Please report this error with the following information:\nRequest: {}", header.id);
		resp.error = err;
	}
	else
	{
		const auto& decryptedGme = BfCrypt::ReadGME(gme, handler.key);
		if (!decryptedGme.has_value())
		{
			GmeError err{};
			err.cmd = GmeErrorCommand::Close;
			err.flag = GmeErrorFlags::IsInError;
			err.message = std::format("Unable to decode request! Please report this error with the following information:\nRequest: {}", header.id);
			resp.error = err;
		}
		else
		{
			const auto& inputJson = decryptedGme.value();
			logReq() << inputJson;

			try
			{
				const auto& outputJson = co_await handler.func(session, inputJson);

				if (!outputJson.empty())
				{
					resp.body = BfCrypt::BuildGME(outputJson, handler.key);
				}

				if (!resp.body.has_value())
				{
					GmeError err{};
					err.cmd = GmeErrorCommand::Close;
					err.flag = GmeErrorFlags::IsInError;
					err.message = std::format("Unable to encode request! Please report this error with the following information:\nRequest: {}", header.id);
					resp.error = err;
				}
			}
			catch (const drogon::orm::DrogonDbException& ex)
			{
				GmeError err{};
				err.cmd = GmeErrorCommand::Close;
				err.flag = GmeErrorFlags::IsInError;
				err.message = std::format("Unable to run database query! Please report this error with the following information:\nRequest: {}\nError: {}", header.id, ex.base().what());
				resp.error = err;
			}
		}
	}

	co_return resp;
}
