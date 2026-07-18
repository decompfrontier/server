#pragma once

/*!
* Endpoints consumed by the offline-proxy libcurl shim (not by the BF
* client itself). These deliver server-owned configuration that the
* client-side hooks need at startup.
*
* <p>URL prefix: /offline_mod/
*/
class OfflineModController : public drogon::HttpController<OfflineModController>
{
public:
	/*!
	* Returns the desired client render-loop cap as a bare integer in
	* the response body (e.g. "60"). Read by the offline-proxy FPS hook
	* on its first MyRender call; falls back to the proxy's compile-time
	* default if this endpoint is unreachable.
	* @param[in] rq HTTP request
	* @param[in] callback Callback to send the response
	*/
	void HandleFpsCap(const drogon::HttpRequestPtr& rq, std::function<void(const drogon::HttpResponsePtr&)>&& callback);

	METHOD_LIST_BEGIN
		ADD_METHOD_TO(OfflineModController::HandleFpsCap, "/offline_mod/fps_cap", drogon::Get);
	METHOD_LIST_END
};
