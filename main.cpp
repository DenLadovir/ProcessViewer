#define _WIN32_WINNT 0x0A00
#define WINVER 0x0A00

#include <windows.h>

#include <iostream>
#include <algorithm>
#include <atomic>

#include "Dashboard.h"
#include "ProcessManager.h"

std::atomic_bool running{true};

BOOL WINAPI consoleHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT ||
        signal == CTRL_BREAK_EVENT ||
        signal == CTRL_CLOSE_EVENT)
    {
        running = false;
        return TRUE;
    }

    return FALSE;
}

int handleInput(int currentPage, int totalPages)
{
    static bool rightWasPressed = false;
    static bool leftWasPressed = false;

    bool rightPressed =
            (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;

    bool leftPressed =
            (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;

    if (rightPressed && !rightWasPressed)
    {
        currentPage = std::min(
                currentPage + 1,
                totalPages - 1
        );
    }

    if (leftPressed && !leftWasPressed)
    {
        currentPage = std::max(
                currentPage - 1,
                0
        );
    }

    rightWasPressed = rightPressed;
    leftWasPressed = leftPressed;

    return currentPage;
}

int main()
{
    HANDLE originalConsole =
            GetStdHandle(STD_OUTPUT_HANDLE);

    if (originalConsole == INVALID_HANDLE_VALUE ||
        originalConsole == nullptr)
    {
        std::wcerr << L"Cannot get console handle.\n";
        return 1;
    }

    HANDLE dashboardConsole = CreateConsoleScreenBuffer(
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            CONSOLE_TEXTMODE_BUFFER,
            nullptr
    );

    if (dashboardConsole == INVALID_HANDLE_VALUE)
    {
        std::wcerr << L"Cannot create console screen buffer.\n";
        return 1;
    }

    if (!SetConsoleActiveScreenBuffer(dashboardConsole))
    {
        std::wcerr << L"Cannot activate screen buffer.\n";
        CloseHandle(dashboardConsole);
        return 1;
    }

    if (!SetConsoleCtrlHandler(consoleHandler, TRUE))
    {
        SetConsoleActiveScreenBuffer(originalConsole);
        CloseHandle(dashboardConsole);

        std::wcerr << L"Cannot register console handler.\n";
        return 1;
    }

    ProcessManager manager;

    int currentPage = 0;

    while (running)
    {
        std::vector<ProcessInfo> processes = manager.enumerate();

        CONSOLE_SCREEN_BUFFER_INFO csbi{};

        if (!GetConsoleScreenBufferInfo(dashboardConsole, &csbi))
            break;

        const int height =
                csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

        const int startRow = 9;

        const int pageSize = std::max(1, height - startRow - 2);

        const int totalPages = std::max(
                1,
                static_cast<int>(
                        (processes.size() + pageSize - 1) / pageSize
                )
        );

        currentPage = std::min(currentPage, totalPages - 1);

        currentPage = handleInput(currentPage, totalPages);

        if(!renderDashboard(dashboardConsole, processes, currentPage))
            break;

        Sleep(500);
    }

    SetConsoleActiveScreenBuffer(originalConsole);

    SetConsoleCtrlHandler(consoleHandler, FALSE);

    CloseHandle(dashboardConsole);

    return 0;
}
