#pragma once

#include <drogon/orm/DbClient.h>
#include <drogon/HttpAppFramework.h>

#define MIGRATION_NS_BEGIN namespace Migrations {
#define MIGRATION_NS_END }

struct IMigration
{
	virtual void execute() = 0;
	virtual const char* getName() const = 0;
};
