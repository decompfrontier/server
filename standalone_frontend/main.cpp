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
            .registerBeginningAdvice([]()
                {
                    auto db = drogon::app().getDbClient("gme");

                    // 1. Create all tables and run any pending migrations.
                    System::Instance().RunMigrations(db);

                    // 2. DEV: top up any users whose unit roster is below the full
                    //    seed count. No-op when DEV_SKIP_TUTORIAL=0.
                    System::Instance().DevValidateAndSeedUnits(db);

                    // 3. Resolve and cache the real user ID so GmeController can
                    //    route all incoming requests without a per-request DB lookup.
                    System::Instance().ResolveActiveUser(db);
                })
            .run();
    }
    catch (const std::exception& ex)
    {
        printf("EXCEPTION: %s\n", ex.what());
    }

    return 0;
}