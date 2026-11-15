#include "Dashboard.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

template<typename T>
std::wstring toWideString(T value)
{
    std::wostringstream stream;
    stream << value;
    return stream.str();
}

bool renderDashboard(
        HANDLE console,
        const std::vector<ProcessInfo>& processes,
        int currentPage
)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi{};

    if (!GetConsoleScreenBufferInfo(console, &csbi))
        return false;

    const SHORT width =
            csbi.srWindow.Right - csbi.srWindow.Left + 1;

    const SHORT height =
            csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    if (width <= 0 || height <= 0)
        return false;

    std::vector<CHAR_INFO> buffer(
            static_cast<size_t>(width) * height
    );

    for (auto& cell : buffer)
    {
        cell.Char.UnicodeChar = L' ';
        cell.Attributes = csbi.wAttributes;
    }

    auto writeLine = [&](int row, const std::wstring& text)
    {
        if (row < 0 || row >= height)
            return;

        const int count = std::min(
                static_cast<int>(text.size()),
                static_cast<int>(width)
        );

        for (int col = 0; col < count; ++col)
        {
            buffer[row * width + col].Char.UnicodeChar =
                    text[col];
        }
    };

    writeLine(0, L"========================================");
    writeLine(1, L"       REAL-TIME PROCESS MONITOR");
    writeLine(2, L"========================================");

    writeLine(4, L"Time: " + getCurrentTime());

    writeLine(
            5,
            L"Active processes: " +
                    toWideString(processes.size())
    );

    writeLine(7,
              L"PID       PROCESS NAME                         START TIME");

    writeLine(8,
              L"------------------------------------------------------------");

    int row = 9;

    const int startRow = 9;
    const int availableRows = height - startRow - 2;

    const int pageSize = std::max(1, availableRows);

    const int totalPages = std::max(
            1,
            static_cast<int>(
                    (processes.size() + pageSize - 1) / pageSize
            )
    );

    const int startIndex = currentPage * pageSize;

    //int row = startRow;

    for (int i = startIndex;
         i < std::min(
                 startIndex + pageSize,
                 static_cast<int>(processes.size())
         );
         ++i)
    {
        std::wostringstream line;

        line << std::left
             << std::setw(13) << processes[i].pid
             << std::setw(36) << processes[i].name
             << processes[i].startTime;

        writeLine(row++, line.str());
    }

    const int shown = std::min(
            startIndex + pageSize,
            static_cast<int>(processes.size())
    );

    writeLine(
            height - 2,
            L"Page " + toWideString(currentPage + 1) +
            L" / " + toWideString(totalPages) +
            L" | Use LEFT / RIGHT arrows"
    );

    writeLine(
            height - 1,
            L"Showing " +
                    toWideString(processes.empty() ? 0 : startIndex + 1) +
            L"-" +
                    toWideString(shown) +
            L" of " +
                    toWideString(processes.size()) +
            L" processes"
    );

    // Записываем кадр в экранный буфер
    SMALL_RECT target = csbi.srWindow;

    COORD bufferSize = { width, height };
    COORD bufferOrigin = { 0, 0 };

    if (!WriteConsoleOutputW(
            console,
            buffer.data(),
            bufferSize,
            bufferOrigin,
            &target
    ))
    {
        return false;
    }

    return true;
}