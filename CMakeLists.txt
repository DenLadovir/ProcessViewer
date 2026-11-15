cmake_minimum_required(VERSION 3.28)
project(ProcessViewer)

set(CMAKE_CXX_STANDARD 26)

add_executable(ProcessViewer
        main.cpp
        Dashboard.cpp
        ProcessManager.cpp
)

target_link_libraries(ProcessViewer PRIVATE Kernel32)