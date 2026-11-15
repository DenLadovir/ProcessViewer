#include "ProcessManager.h"

#include <tlhelp32.h>
#include <algorithm>
#include <iomanip>
#include <sstream>

std::wstring getProcessStartTime(DWORD pid, ULONGLONG& startTimeValue)
{
    startTimeValue = 0;

    HANDLE hProcess = OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION,
            FALSE,
            pid
    );

    if (!hProcess)
        return L"--:--:--";

    FILETIME creationTime, exitTime, kernelTime, userTime;

    if (!GetProcessTimes(
            hProcess,
            &creationTime,
            &exitTime,
            &kernelTime,
            &userTime
    ))
    {
        CloseHandle(hProcess);
        return L"--:--:--";
    }

    CloseHandle(hProcess);

    ULARGE_INTEGER rawTime;
    rawTime.LowPart = creationTime.dwLowDateTime;
    rawTime.HighPart = creationTime.dwHighDateTime;

    startTimeValue = rawTime.QuadPart;

    SYSTEMTIME utcTime, localTime;

    FileTimeToSystemTime(&creationTime, &utcTime);
    SystemTimeToTzSpecificLocalTime(
            nullptr,
            &utcTime,
            &localTime
    );

    std::wostringstream stream;

    stream << std::setfill(L'0')
           << std::setw(2) << localTime.wHour << L":"
           << std::setw(2) << localTime.wMinute << L":"
           << std::setw(2) << localTime.wSecond;

    return stream.str();
}

std::wstring getCurrentTime()
{
    SYSTEMTIME time{};
    GetLocalTime(&time);

    std::wostringstream stream;

    stream << std::setfill(L'0')
           << time.wYear << L"-"
           << std::setw(2) << time.wMonth << L"-"
           << std::setw(2) << time.wDay << L" "
           << std::setw(2) << time.wHour << L":"
           << std::setw(2) << time.wMinute << L":"
           << std::setw(2) << time.wSecond;

    return stream.str();
}

std::vector<ProcessInfo> ProcessManager::enumerate()
{
    std::vector<ProcessInfo> processes;

    HANDLE snapshot = CreateToolhelp32Snapshot(
            TH32CS_SNAPPROCESS, 0
    );

    if (snapshot == INVALID_HANDLE_VALUE)
        return processes;

    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);

    if (Process32FirstW(snapshot, &entry))
    {
        do
        {
            ULONGLONG startTimeValue = 0;

            std::wstring startTime = getProcessStartTime(
                    entry.th32ProcessID,
                    startTimeValue
            );

            processes.push_back({
                                        entry.th32ProcessID,
                                        entry.szExeFile,
                                        startTime,
                                        startTimeValue
                                });

        } while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);

    std::stable_sort(
            processes.begin(),
            processes.end(),
            [](const ProcessInfo& a, const ProcessInfo& b)
            {
                if (a.startTimeValue == 0)
                    return false;

                if (b.startTimeValue == 0)
                    return true;

                return a.startTimeValue > b.startTimeValue;
            }
    );

    return processes;
}