#ifndef PROCESSVIEWER_DASHBOARD_H
#define PROCESSVIEWER_DASHBOARD_H

#include <windows.h>
#include <vector>

#include "ProcessManager.h"

bool renderDashboard(
        HANDLE console,
        const std::vector<ProcessInfo>& processes,
        int currentPage
);

#endif //PROCESSVIEWER_DASHBOARD_H
