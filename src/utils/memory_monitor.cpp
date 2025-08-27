#undef byte
#include "memory_monitor.hpp"
#ifdef _WIN32
    #include <windows.h>
    #include <psapi.h>
#else
    #include <sys/resource.h>
    #include <unistd.h>
#endif

std::size_t getAvailableSystemMemory() {
    #ifdef _WIN32
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(status);
        if (GlobalMemoryStatusEx(&status)) {
            return (std::size_t)status.ullAvailPhys; // bytes
        }
        return 0;
    #else
        long pages = sysconf(_SC_AVPHYS_PAGES);
        long page_size = sysconf(_SC_PAGE_SIZE);
        if (pages != -1 && page_size != -1) {
            return (std::size_t)pages * (std::size_t)page_size;
        }
        return 0;
    #endif
}