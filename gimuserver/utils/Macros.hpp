#pragma once

class GimuServer; // fwd...

/*!
* Gets the server singleton
* @return The server
*/
static inline GimuServer* theServer() { return drogon::app().getPlugin<GimuServer>(); }

#define LOG_REQ (theServer()->httpLog())
#define LOG_DLC (theServer()->dlcLog())
#define GME_DB (drogon::app().getDbClient())
#define SYS_ROOT (drogon::app().getDocumentRoot())

