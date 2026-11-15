#ifndef PROCESSVIEWER_PROCESSMANAGER_H
#define PROCESSVIEWER_PROCESSMANAGER_H

#include <windows.h>
#include <string>
#include <vector>

struct ProcessInfo
{
    DWORD pid;
    std::wstring name;
    std::wstring startTime;
    ULONGLONG startTimeValue = 0;
};

class ProcessManager
{
public:
    std::vector<ProcessInfo> enumerate();
};

std::wstring getCurrentTime();

#endif // PROCESSVIEWER_PROCESSMANAGER_H