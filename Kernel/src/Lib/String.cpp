#include "Lib/String.hpp"

void memset(void* dest, u8 value, u32 size) {
    u8* ptr = (u8*)dest;

    for(u32 i = 0; i < size; i++) {
        ptr[i] = value;
    }
}
