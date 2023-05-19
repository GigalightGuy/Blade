#pragma once

#include "Platform.hpp"

#ifdef BLADE_DEBUG
    #if defined(BLD_PLATFORM_WINDOWS)
        #define BLD_DEBUGBREAK() __debugbreak()
    #elif defined(BLD_PLATFORM_LINUX)
        #include <signal.h>
        #define HZ_DEBUGBREAK() raise(SIGTRAP)
    #else
        #error "Platform doesn't support debugbreak yet!"
    #endif
#else
    #define BLD_DEBUGBREAK()
#endif


#include "Log.hpp"

#define BLD_CORE_ASSERT(check, ...) if (!(check)) { BLD_CORE_CRITICAL(__VA_ARGS__); BLD_DEBUGBREAK(); }