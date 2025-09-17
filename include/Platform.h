#pragma once

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define DRD_WINDOWS 1
#elif defined(__linux__)
    #define DRD_LINUX 1
#else
    #error "Unsupported platform"
#endif