#pragma once

#ifdef _WIN32
#include <QtGlobal>
#define ASSERT Q_ASSERT
#elif __linux__
#include <csignal>

#ifdef DEBUG
    #ifdef __linux__
        #define ASSERT(x) if(!(x)) { /*LOG_ASSERT(#x);*/ std::raise(SIGTRAP); }
    #elif _WIN32
        #define ASSERT(x) do { \
            if (!(x) && (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, #x))) \
                _CrtDbgBreak(); \
        } while (0)
    #endif
#else
    #define ASSERT(expr) ((void)0)
#endif
#endif

