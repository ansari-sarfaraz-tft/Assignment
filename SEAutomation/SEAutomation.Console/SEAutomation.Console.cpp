#include <Windows.h>
#include <chrono>
#include "logger.h"

typedef bool(*RUNFN)();

int main()
{
    Logger logger;

    auto start =
        std::chrono::steady_clock::now();

    logger.Write("Started");

    HMODULE dll =
        LoadLibraryA("SEAutomation.Library.dll");

    if (!dll)
    {
        logger.Write("DLL load failed");
        return 1;
    }

    RUNFN run =
        (RUNFN)GetProcAddress(
            dll,
            "RunAutomation");

    bool result = false;

    if (run)
    {
        result = run();
    }

    FreeLibrary(dll);

    auto end =
        std::chrono::steady_clock::now();

    auto duration =
        std::chrono::duration_cast
        <std::chrono::milliseconds>(
            end - start);

    logger.Write(
        result ? "Success" : "Failure");

    logger.Write(
        "Duration(ms): "
        + std::to_string(duration.count()));

    return result ? 0 : 1;
}