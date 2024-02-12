#pragma once

#include <drogon/orm/DbClient.h>
#include <drogon/HttpAppFramework.h>

#define GME_DB drogon::app().getDbClient("gme")
