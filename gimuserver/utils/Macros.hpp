#pragma once

class GimuServer; // fwd...

/*!
* Gets the server singleton
* @return The server
*/
static inline GimuServer* theServer() { return drogon::app().getPlugin<GimuServer>(); }

/*!
* Gets the DLC logger
* @return DLC logger
*/
static inline DumpLog& logDlc() { return theServer()->dlcLog(); }

/*!
* Gets the default database instance
* @return The database
*/
static inline drogon::orm::DbClientPtr theDb() { return drogon::app().getDbClient(); }

/*!
* Gets the document root
* @return Document root
*/
static inline std::string getDocumentRoot() { return drogon::app().getDocumentRoot(); }
