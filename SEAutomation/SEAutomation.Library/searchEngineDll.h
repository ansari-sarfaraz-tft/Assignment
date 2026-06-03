#pragma once

#ifdef EDGEAUTOMATIONDLL_EXPORTS
#define EDGE_API __declspec(dllexport)
#else
#define EDGE_API __declspec(dllimport)
#endif

extern "C"
{
    EDGE_API bool ExecuteAutomation();
}