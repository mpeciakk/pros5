#pragma once
#include <stdarg.h>

void panic(const char* __restrict format, ...);

class Log {
public:
    static bool isFramebufferInitialized;

    static void print(char ch);
    static void prints(const char* str);
    static void kprint(char ch);
    static void kprints(const char* str);
    static void printf(const char* __restrict format, va_list parameters, bool serial = false);
    static void log(const char* __restrict format, ...);
    static void debug(const char* __restrict format, ...);
    static void warn(const char* __restrict format, ...);
    static void error(const char* __restrict format, ...);
    static void fatal(const char* __restrict format, ...);
};
