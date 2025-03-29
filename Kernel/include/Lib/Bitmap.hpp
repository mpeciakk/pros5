#pragma once

#include "Types.hpp"

class Bitmap {
public:
    u8* bitmap;
    u32 size;

    inline void setBit(u32 index) {
        bitmap[index / 8] |= (1 << (index % 8));
    }

    inline void clearBit(u32 index) {
        bitmap[index / 8] &= ~(1 << (index % 8));
    }

    inline bool getBit(u32 index) {
        return (bitmap[index / 8] & (1 << (index % 8))) != 0;
    }

    void clear();
};