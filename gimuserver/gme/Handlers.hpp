#pragma once

/*!
* Class to hold results of an handler.
*/
struct HandleResult
{
	/*!
	* Success JSON.
	*/
	std::string successJson;

	/*!
	* Error message.
	*/
	std::string errorMsg;

	/*!
	* Exception message.
	*/
	std::string exceptionMsg;

	/*!
	* Checks if it's in error.
	* @return true if the result is an error, otherwise false
	*/
	constexpr bool isError() const { return !errorMsg.empty(); }

	/*!
	* Create an error result
	* @param error Error message
	* @param ex Exception
	* @return An error result
	*/
	static inline HandleResult error(const std::string& error, const std::string& ex = "") {
		return HandleResult("", error, ex);
	}

	/*!
	* Create a successfull result
	* @param success Success message
	* @return A success result
	*/
	static inline HandleResult success(const std::string& success) {
		return HandleResult(success, "", {});
	}

private:
	/*!
	* Private constructor for a result.
	* @param success Success JSON (or empty in case of no success)
	* @param error Error message (or empty in case of a success)
	* @param ex Optional glaze error
	*/
	explicit HandleResult(const std::string& success, const std::string& error, const std::string& ex) : successJson(success), errorMsg(error), exceptionMsg(ex)
	{}
};

/*!
* Defines how an handler function is constructer.
* @param session Drogon session handler
* @param json Decrypted input JSON
* @return Output decrypted JSON
*/
using HandlerFunc = std::function<drogon::Task<HandleResult>(drogon::SessionPtr session, std::string json)>;

/*!
* Defines the prototype of an handler.
* @param name Name of the handler
*/
#define HANDLE(name) drogon::Task<HandleResult> name(drogon::SessionPtr session, std::string json)

/*!
* Defines the prototype of an handler with namespace.
* @param name Name of the handler
*/
#define HANDLEF(name) HANDLE(GmeHandlers::name)

// list all available handlers
namespace GmeHandlers
{
	HANDLE(Initialize);
	HANDLE(BadgeInfo);
	HANDLE(ControlCenterEnter);
	HANDLE(DeckEdit);
	HANDLE(FriendGet);
	HANDLE(GatchaList);
	HANDLE(HomeInfo);
	HANDLE(MissionStart);
	HANDLE(UpdateInfoLight);
	HANDLE(UserInfo);
}
