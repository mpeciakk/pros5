#pragma once

class Log {
public:
    static bool isFramebufferInitialized;

    static void log(const char* __restrict format, ...);
    static void debug(const char* __restrict format, ...);
    static void warn(const char* __restrict format, ...);
    static void error(const char* __restrict format, ...);
    static void fatal(const char* __restrict format, ...);
};
