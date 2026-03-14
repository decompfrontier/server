#include <drogon/drogon.h>

#include <gimuserver/App.hpp>
#include <gimuserver/db/MigrationManager.hpp>
#include <gimuserver/drogon/GimuServer.hpp>

int main(int argc, char** argv)
{
#ifdef _WIN32
    SetConsoleTitleW(L"GimuFrontier standalone server");
#endif

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

    drogon::HttpAppFramework::instance().getLoop()->queueInLoop([]()
        {
            drogon::HttpAppFramework::instance().quit();
    });

/*#ifdef _WIN32
    TerminateProcess(GetCurrentProcess(), 0);
#endif*/
    return 0;
}
