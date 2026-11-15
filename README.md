# ProcessViewer

A real-time Windows process monitor written in C++ using the WinAPI.

## Features

- Lists currently running processes.
- Displays process ID and executable name.
- Shows process start time.
- Sorts processes by start time (newest first).
- Automatically refreshes the process list every 500 ms.
- Supports pagination using the left and right arrow keys.
- Uses a separate console screen buffer to update the dashboard.

## Technologies

- C++
- Windows API (WinAPI)
- CMake

## Controls:
Key	Action
←	Previous page
→	Next page
Ctrl+C	Exit

Platform: Windows

## Build

```bash
cmake -S . -B build
cmake --build build
=======
Real-time Windows process monitor written in C++
>>>>>>> 7de981ccc2bcfceceabb09c69792825aaa1e68ce
