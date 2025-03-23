#include "Lib/Log.hpp"
#include "Drivers/Framebuffer.hpp"
#include <stdarg.h>
#include "Hardware/Port.hpp"

void print(char ch) {
    FramebufferConsole::instance().putChar(ch);
}

void kprint(char ch) {
    static Port8Bit port(0x3F8);
    port.write(ch);
}

int itoa(int value, char* str, int base) {
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return 1;
    }
    
    int negative = 0;
    if (value < 0 && base == 10) {
        negative = 1;
        value = -value;
    }
    
    int i = 0;
    while (value != 0) {
        int remainder = value % base;
        str[i++] = (remainder < 10) ? remainder + '0' : remainder - 10 + 'a';
        value /= base;
    }
    
    if (negative) {
        str[i++] = '-';
    }
    
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
    
    str[i] = '\0';
    
    return i;
}


void printf(const char* __restrict format, va_list parameters, bool serial = false) {
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
            int i = itoa(num, buffer, 10);
            for (int j = 0; j < i; j++) {
                printFunction(buffer[j]);
            }
        } else if (*format == 'x') {
            format++;
            int num = va_arg(parameters, int);
            char buffer[32];
            int i = itoa(num, buffer, 16);
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