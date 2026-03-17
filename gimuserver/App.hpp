#pragma once

// avoid random windows definitions which breaks glaze
#define NOMINMAX 1

// trantor
#include <trantor/utils/AsyncFileLogger.h>
#include <trantor/utils/NonCopyable.h>
#include <trantor/utils/Logger.h>
// drogon
#include <drogon/drogon.h>
#include <drogon/Session.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>
#include <drogon/utils/Utilities.h>

// packets
#include <gimuserver/packets/all.hpp>

// main system
#include <gimuserver/drogon/GimuServer.hpp>
// main controllers
#include <gimuserver/controller/DlsController.hpp>
#include <gimuserver/controller/AccountController.hpp>
#include <gimuserver/controller/BfWebController.hpp>
#include <gimuserver/controller/GmeController.hpp>
// utility
#include <gimuserver/utils/Drogon.hpp>
#include <gimuserver/utils/Macros.hpp>
