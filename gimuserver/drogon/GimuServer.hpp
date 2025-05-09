#pragma once

#include <drogon/plugins/Plugin.h>
#include <gimuserver/drogon/ServerCache.hpp>

/*!
* Implements the plugin of the system server. This is used to contain and caches everything.
*/
class GimuServer final : public drogon::Plugin<GimuServer>
{
public:
    /*!
    * Constructs a GimuServer object.
    */
    GimuServer();

    /*!
    * This method is called by drogon to initialize and start the plugin.
    * @param[in] config Plugin configuration
    */
    void initAndStart(const Json::Value& config) override;

    /*!
    * This method is called by drogon to shutdown the plugin.
    */
    void shutdown() override;

    /*!
    * Gets the server cache.
    * @return Server cache
    */
    inline auto& cache() { return m_cache; }

    /*!
    * Gets the HTTP request/response logger.
    * @return HTTP logger
    */
    inline auto& httpLog() const { return m_http_log; }

    /*!
    * Gets the DLC 404 logger.
    * @return DLC logger
    */
    inline auto& dlcLog() const { return m_dlc_error_log; }

private:
    /*!
    * HTTP log file.
    */
    DumpLog m_http_log;

    /*!
    * DLC error file.
    */
    DumpLog m_dlc_error_log;

    /*!
    * Enable debug logging.
    */
    bool m_have_log;

    /*!
    * Server cache.
    */
    ServerCache m_cache;
};
