#pragma once

#ifdef _WIN32
    #define BLD_PLATFORM_WINDOWS
#elif defined(__linux__)
    #define BLD_PLATFORM_LINUX
#endif