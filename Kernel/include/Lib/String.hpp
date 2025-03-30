#pragma once

#include "Types.hpp"

void memset(void* dest, u8 value, u32 size);
int itos(i32 value, char* str, u32 base);
int utos(u32 value, char* str, u32 base);
