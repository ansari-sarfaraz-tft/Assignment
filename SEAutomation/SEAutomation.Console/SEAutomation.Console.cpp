#include <Windows.h>
#include <chrono>
#include <ctime>
#include <sstream>
#include "logger.h"

typedef bool(*RUNFN)();

static std::string TimeToString(std::chrono::steady_clock::time_point tp)
{
    auto now_c = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now_c);

    std::tm tm;
    localtime_s(&tm, &t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

int main()
{
    Logger logger;

    auto start = std::chrono::steady_clock::now();
    logger.Write("Start time: " + TimeToString(start));

    logger.Write("Status: Running");

    HMODULE dll = LoadLibraryA("SEAutomation.Library.dll");

    if (!dll)
    {
        logger.Write("Status: Failed (DLL load failed)");
        logger.Write("End time: " + TimeToString(std::chrono::steady_clock::now()));
        logger.Write("Total time(ms): 0");
        return 1;
    }

    RUNFN run = (RUNFN)GetProcAddress(dll, "RunAutomation");

    bool result = false;

    if (run)
    {
        result = run();
        logger.Write(std::string("Status: ") + (result ? "Success" : "Failure"));
    }
    else
    {
        logger.Write("Status: Failed (function not found)");
    }

    auto end = std::chrono::steady_clock::now();

    logger.Write("End time: " + TimeToString(end));

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    logger.Write("Total time(ms): " + std::to_string(duration.count()) + "\n\n");

    FreeLibrary(dll);

    return result ? 0 : 1;
}