#include "PathHelper.hpp"

#ifdef _WIN32
    #include <windows.h>
#elif __linux__
    #include <unistd.h>
    #include <limits.h>
#elif __APPLE__
    #include <mach-o/dyld.h>
#endif

std::string Utilities::getExecutablePath() {
    char buffer[1024];

    #ifdef _WIN32
        GetModuleFileNameA(nullptr, buffer, sizeof(buffer));
    #elif __linux__
        ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer));

        if (count == -1) return "";
        buffer[count] = '\0';
    #elif __APPLE__
        uint32_t size = sizeof(buffer);

        if (_NSGetExecutablePath(buffer, &size) != 0)
            return "";
    #endif

    std::string path(buffer);

    return path.substr(0, path.find_last_of("/\\"));
}
