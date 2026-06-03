#include "logger.h"
#include <fstream>
#include <filesystem>
#include <cstdlib>

void Logger::Write(const std::string& message)
{
    char* appData = nullptr;
    size_t len = 0;

    _dupenv_s(&appData, &len, "APPDATA");

    if (!appData)
        return;

    std::filesystem::path path(appData);

    free(appData);

    path /= "EdgeAutomation";

    std::filesystem::create_directories(path);

    path /= "automation.log";

    std::ofstream file(path, std::ios::app);

    if (file.is_open())
    {
        file << message << std::endl;
    }
}