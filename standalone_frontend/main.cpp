#include <drogon/drogon.h>
#include <cstdio>
#include <AppVersion.hpp>

#include <gimuserver/core/System.hpp>
#include <gimuserver/core/Controllers.hpp>

#include <json/value.h>

int main(int argc, char** argv)
{
    printf("GimuFrontier - C++ Game Server for Brave Frontier\n");
    printf(u8"Revision %d (%s)\n", SERVER_REVISION, SERVER_COPYRIGHT);
    printf("Report any issues to %s\n\n", SERVER_ISSUE_URL);

    try
    {
        System::Instance().LoadSystemConfig("./gimuconfig.json");

        drogon::app()
            .loadConfigFile("./config.json")
            .createDbClient("sqlite3", "", 0, "", "", "", 1, System::Instance().GetDbPath(), "gme", false, "utf-8")
            .registerBeginningAdvice([]() {
            System::Instance().RunMigrations(drogon::app().getDbClient("gme"));
            })
            .run()
        ;
    }
    catch (const std::exception& ex)
    {
        printf("EXCEPTION: %s\n", ex.what());
    }

    return 0;
}
