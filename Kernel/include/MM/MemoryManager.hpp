#pragma once

#include "Lib/Types.hpp"
#include "Multiboot/multiboot.h"
#include "Lib/Bitmap.hpp"

class MemoryManager {
public:
    static MemoryManager& instance() {
        static MemoryManager memoryManager;
        return memoryManager;
    }

    MemoryManager() = default;

    void init(u32 addr);
    void getPage(u32 count);
    void printAllocationTable();
    void printBuddyVisualization();

private:
    static constexpr u32 MIN_BLOCK_SIZE = 4096;
    static constexpr u32 NUM_LEVELS = 5;
    static constexpr u32 MAX_BLOCK_SIZE = MIN_BLOCK_SIZE << (NUM_LEVELS - 1);
    static constexpr u32 MAX_MEMORY_MB = 64;
    static constexpr u32 MAX_BLOCKS_PER_LEVEL = (MAX_MEMORY_MB * 1024 * 1024) / MIN_BLOCK_SIZE;
    static constexpr u32 MAX_BITMAP_SIZE = (MAX_BLOCKS_PER_LEVEL + 7) / 8;

    u32 totalMemory;
    void initPMM(u32 addr);

    // Buddy allocator
    u8 bitmapAddress[NUM_LEVELS][MAX_BITMAP_SIZE];
    Bitmap bitmaps[NUM_LEVELS];

    void initBuddyAllocator(u32 addr);
    u32 indexToLevel(u32 sourceLevel, u32 targetLevel, u32 index);
    u32 getNearestFreeblock(u32 level);
};
