#include <drogon/drogon.h>
#include <cstdio>
#include <AppVersion.h>
#include <DlsController.hpp>

int main()
{
    printf("GimuFrontier - C++ Game Server for Brave Frontier\n");
    printf("Version %s (Copyright (C) %s %s)\n", APP_VERSION, APP_YEAR, APP_AUTHOR);
    drogon::app()
        .loadConfigFile("./config.json")
        .run();
    return 0;
}
