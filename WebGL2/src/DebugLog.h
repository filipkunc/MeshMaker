#pragma once

// Debug logging macros - controlled by MESHMAKER_DEBUG_LOG
//
// Define MESHMAKER_DEBUG_LOG at compile time to enable verbose debug output:
//   cmake -DCMAKE_CXX_FLAGS="-DMESHMAKER_DEBUG_LOG" ...
// Or toggle the CMake option MESHMAKER_DEBUG_LOGGING=ON
//
// Usage:
//   DEBUG_LOG("value=%d\n", someValue);
//   DEBUG_COUT("[Tag] message: " << value << std::endl);

#ifdef MESHMAKER_DEBUG_LOG

#include <cstdio>
#include <iostream>

#define DEBUG_LOG(...) printf(__VA_ARGS__)
#define DEBUG_COUT(x) std::cout << x

#else

#define DEBUG_LOG(...) ((void)0)
#define DEBUG_COUT(x) ((void)0)

#endif
