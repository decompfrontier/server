#pragma once

#include <drogon/orm/DbClient.h>

/*!
* Simple class to create database classes without a mapper. (The mapper wasn't used because it didn't seem like to support sqlite)
*/
class MigrationManager final : public trantor::NonCopyable
{
public:
	/*!
	* Runs the migrations before the system starts up.
	* @param[in] p Drogon database client
	*/
	static void RunMigrations(drogon::orm::DbClientPtr p);

private:
	/*!
	* Private constructor.
	*/
	MigrationManager() = default;
};
