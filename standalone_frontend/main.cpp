#include <drogon/drogon.h>
#include <cstdio>
#include <AppVersion.hpp>

#include <gimuserver/core/System.hpp>
#include <gimuserver/core/Controllers.hpp>

int main()
{
    printf("GimuFrontier - C++ Game Server for Brave Frontier\n");
    printf(u8"Revision %d (%s)\n", SERVER_REVISION, SERVER_COPYRIGHT);
    printf("Report any issues to %s\n\n", SERVER_ISSUE_URL);

    try
    {
        if (!System::Instance().LoadSystemConfig("./gimuconfig.json"))
        {
            printf("Unable to load server config!\n");
            return -1;
        }

#if 0 // TODO: fix!!!
        {
            auto p = drogon::orm::DbClient::newSqlite3Client("filename=" + System::Instance().GetDbPath(), 1);
            System::Instance().RunMigrations(p);
            p->closeAll();
        }
#endif

        drogon::app()
            .loadConfigFile("./config.json")
            .createDbClient("sqlite3", "", 0, "", "", "", 1, System::Instance().GetDbPath(), "gme", false, "utf-8")
            .enableSession(System::Instance().GetSessionTimeout())
            .run()
        ;
    }
    catch (const std::exception& ex)
    {
        printf("EXCEPTION: %s\n", ex.what());
    }

    return 0;
}
