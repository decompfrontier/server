#include <drogon/drogon.h>

#include <gimuserver/App.hpp>
#include <gimuserver/db/MigrationManager.hpp>
#include <gimuserver/drogon/GimuServer.hpp>

// The debug CLI is a local-only dev tool (git-ignored). When DebugCli.cpp is
// present, standalone_frontend/CMakeLists.txt defines BF_DEBUG_CLI; upstream
// builds without the file simply omit it.
#ifdef BF_DEBUG_CLI
#include "DebugCli.hpp"
#endif

#include <filesystem>

int main(int argc, char** argv)
{
    // Unbuffered stdout so log lines reach redirected files immediately -
    // otherwise a wedged/killed headless run loses everything still sitting
    // in the CRT block buffer and boot failures become undiagnosable.
    setvbuf(stdout, nullptr, _IONBF, 0);

#ifdef BF_DEBUG_CLI
    // CLI-client mode: launched by the server to host the debug console window.
    // Must be checked before any Drogon / config setup so this process never
    // starts the web server or runs migrations.
    if (argc >= 3 && std::string(argv[1]) == "--debug-cli")
    {
        RunDebugCliClient(argv[2]);
        return 0;
    }
#endif

#ifdef _WIN32
    SetConsoleTitleW(L"GimuFrontier standalone server");
#endif

    // Config path resolution priority:
    //   1. Command-line argument (argv[1]) — explicit always wins.
    //   2. GIMU_DEFAULT_CONFIG_PATH — baked in at compile time for Debug
    //      builds so F5 from VS works without touching the launch config.
    //   3. "./config.json" — fallback for Release / command-line usage where
    //      the caller cd's into deploy/ first (e.g. rebuild.bat).
#ifndef GIMU_DEFAULT_CONFIG_PATH
#define GIMU_DEFAULT_CONFIG_PATH "./config.json"
#endif
    const char* configArg = (argc > 1) ? argv[1] : GIMU_DEFAULT_CONFIG_PATH;

    try
    {
        // Resolve to an absolute path so the chdir below works regardless of
        // what the current directory was at launch.
        auto absConfig = std::filesystem::absolute(configArg);

        // Change into the config file's directory.  Every path in config.json
        // is relative to its own location (mst_root, document_root, etc.) so
        // this makes the server portable without editing config.json.
        std::filesystem::current_path(absConfig.parent_path());

        drogon::app()
            .loadConfigFile(absConfig.filename().string())
            .registerBeginningAdvice([]() {
                auto db = drogon::app().getDbClient();
                MigrationManager::RunMigrations(db);
                // Legacy SeedDefaultUnits/SeedDefaultTown removed: the
                // tutorial (CreateUser + UnitArchiver) provisions the user
                // under the fresh-DB model adopted from upstream dev.
#ifdef BF_DEBUG_CLI
                StartDebugCli();
#endif
            })
            .run();
    }
    catch (const std::exception& ex)
    {
        printf("Fatal exception during execution: %s\n", ex.what());
#ifdef _WIN32
        // Also write to the VS Output window so the message is visible
        // without needing to watch the console window.
        OutputDebugStringA("Fatal exception during execution: ");
        OutputDebugStringA(ex.what());
        OutputDebugStringA("\n");
#endif
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
