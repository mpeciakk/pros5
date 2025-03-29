#include "Lib/Bitmap.hpp"

void Bitmap::clear() {
    for (u32 i = 0; i < size; i++) {
        bitmap[i] = 0;
    }
}