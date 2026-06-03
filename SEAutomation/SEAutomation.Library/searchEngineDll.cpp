#include "pch.h"
#include "searchEngineDll.h"
#include "browserAutomation.h"

extern "C" __declspec(dllexport)
bool RunAutomation()
{
    BrowserAutomation automation;
    return automation.Execute();
}