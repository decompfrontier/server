#include <drogon/drogon.h>
#include <cstdio>
#include <AppVersion.h>
#include <Controllers.hpp>

int main()
{
    printf("GimuFrontier - C++ Game Server for Brave Frontier\n");
    printf("Version %s (Copyright (C) %s %s)\n", APP_VERSION, APP_YEAR, APP_AUTHOR);

    try
    {
        drogon::app()
            .loadConfigFile("./config.json")
            .run();

        LOG_INFO << "Started server!";
    }
    catch (const std::exception& ex)
    {
        printf("EXCEPTION: %s\n", ex.what());
    }

    return 0;
}
