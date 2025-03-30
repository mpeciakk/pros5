#pragma once

#include "Types.hpp"

void memset(void* dest, u8 value, u32 size);
int itos(i32 value, char* str, u32 base);
int utos(u32 value, char* str, u32 base);
int strcmp(const char* str1, const char* str2);
int strncmp(const char* str1, const char* str2, u32 n);
int strlen(const char* str);
void strcpy(char* dest, const char* src);
void strncpy(char* dest, const char* src, u32 n);
void memcpy(void* dest, const void* src, u32 size);
