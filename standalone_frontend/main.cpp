#include <drogon/drogon.h>
#include <AppVersion.hpp>

#include <gimuserver/db/MigrationManager.hpp>
#include <gimuserver/drogon/GimuServer.hpp>

int main(int argc, char** argv)
{
#ifdef _WIN32
    SetConsoleTitleW(L"GimuFrontier standalone server");
#endif
    printf("GimuFrontier standalone %s/%s %s\n\n\n", AppVersion::GIT_COMMIT_HASH, AppVersion::GIT_BRANCH, AppVersion::GIT_DIRTY);

    try
    {
        drogon::app()
            .loadConfigFile("./config.json")
            .registerBeginningAdvice([]() {
                MigrationManager::RunMigrations(drogon::app().getDbClient());
            })
            .run()
        ;
    }
    catch (const std::exception& ex)
    {
        printf("Fatal exception during execution: %s\n", ex.what());
    }

    return 0;
}
