#include "Lib/Log.hpp"
#include "Drivers/Framebuffer.hpp"
#include "Hardware/Port.hpp"
#include "Lib/String.hpp"
#include <stdarg.h>

void Log::print(char ch) {
    FramebufferConsole::instance().putChar(ch);
}

void Log::prints(const char* str) {
    FramebufferConsole::instance().print(str);
}

void Log::kprint(char ch) {
    static Port8Bit port(0x3F8);
    port.write(ch);
}

void Log::kprints(const char* str) {
    for (u32 i = 0; str[i] != '\0'; i++) {
        kprint(str[i]);
    }
}

void Log::printf(const char* __restrict format, va_list parameters, bool serial) {
    auto printFunction = serial ? kprint : print;

    while (*format != '\0') {
        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%') {
                format++;
            }

            u32 amount = 1;

            while (format[amount] && format[amount] != '%') {
                amount++;
            }

            for (u32 i = 0; i < amount; i++) {
                printFunction(format[i]);
            }

            format += amount;
            continue;
        }

        const char* format_begun_at = format++;

        if (*format == 'c') {
            format++;
            char c = (char) va_arg(parameters, int);
            printFunction(c);
        } else if (*format == 'd') {
            format++;
            int num = va_arg(parameters, int);
            char buffer[32];
            int i = itos(num, buffer, 10);
            for (int j = 0; j < i; j++) {
                printFunction(buffer[j]);
            }
        } else if (*format == 'u') {
            format++;
            unsigned int num = va_arg(parameters, unsigned int);
            char buffer[32];
            int i = utos(num, buffer, 10);
            for (int j = 0; j < i; j++) {
                printFunction(buffer[j]);
            }
        } else if (*format == 'x') {
            format++;
            int num = va_arg(parameters, int);
            char buffer[32];
            int i = utos(num, buffer, 16);
            printFunction('0');
            printFunction('x');
            for (int j = 0; j < i; j++) {
                printFunction(buffer[j]);
            }
        } else if (*format == 's') {
            format++;
            const char* str = va_arg(parameters, const char*);
            while (*str) {
                printFunction(*str);
                str++;
            }
        } else {
            format = format_begun_at;
            u32 len = 0;

            while (*format) {
                printFunction(*format);
                format++;
                len++;
            }

            format += len;
        }
    }
}

void Log::log(const char* __restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    printf("[LOG] ", parameters);
    printf(format, parameters);
    print('\n');

    va_end(parameters);
}

void Log::debug(const char* __restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    printf(format, parameters, true);
    kprint('\n');

    va_end(parameters);
}

void Log::warn(const char* __restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    printf("[WARN] ", parameters);
    printf(format, parameters);

    va_end(parameters);
}

void Log::error(const char* __restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    printf("[ERROR] ", parameters);
    printf(format, parameters);

    va_end(parameters);
}

void Log::fatal(const char* __restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    printf("[FATAL] ", parameters);
    printf(format, parameters);

    va_end(parameters);
}

void [[noreturn]] panic(const char* __restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    Log::printf("--------------------------------\n", parameters);
    Log::printf("KERNEL BRUH MOMENT\n", parameters);
    Log::printf(format, parameters);
    Log::printf("\n", parameters);
    Log::printf("--------------------------------\n", parameters);

    Log::printf("--------------------------------\n", parameters, true);
    Log::printf("KERNEL BRUH MOMENT\n", parameters, true);
    Log::printf(format, parameters, true);
    Log::printf("\n", parameters, true);
    Log::printf("--------------------------------\n", parameters, true);

    va_end(parameters);

    while (true) {
        __asm__ volatile("cli");
        __asm__ volatile("hlt");
    }
}
