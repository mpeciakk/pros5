#include "Lib/String.hpp"

void memset(void* dest, u8 value, u32 size) {
    u8* ptr = (u8*) dest;

    for (u32 i = 0; i < size; i++) {
        ptr[i] = value;
    }
}

int itos(i32 value, char* str, u32 base) {
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

int utos(u32 value, char* str, u32 base) {
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return 1;
    }

    int i = 0;
    while (value != 0) {
        int remainder = value % base;
        str[i++] = (remainder < 10) ? remainder + '0' : remainder - 10 + 'a';
        value /= base;
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

int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

int strncmp(const char* str1, const char* str2, u32 n) {
    while (n > 0 && *str1 && (*str1 == *str2)) {
        str1++;
        str2++;
        n--;
    }

    return n == 0 ? 0 : *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

int strlen(const char* str) {
    int len = 0;
    while (*str++) {
        len++;
    }
    return len;
}

void strcpy(char* dest, const char* src) {
    while ((*dest++ = *src++) != '\0');
}

void strncpy(char* dest, const char* src, u32 n) {
    while (n > 0 && (*dest++ = *src++) != '\0') {
        n--;
    }
}

void memcpy(void* dest, const void* src, u32 size) {
    u8* d = (u8*) dest;
    const u8* s = (const u8*) src;

    for (u32 i = 0; i < size; i++) {
        d[i] = s[i];
    }
}
