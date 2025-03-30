#pragma once

#include "Lib/Bitmap.hpp"
#include "Lib/Types.hpp"
#include "Multiboot/multiboot.h"

#define PAGE_SIZE 4096

static inline u32 PAGE_DIRECTORY_INDEX(u32 x) {
    return ((x >> 22) & 0x3ff);
}

static inline u32 PAGE_TABLE_INDEX(u32 x) {
    return ((x >> 12) & 0x3ff);
}

static inline u32 PAGE_GET_PHYSICAL_ADDRESS(u32* x) {
    return (*x & ~0xFFF);
}

static inline u32 GET_FRAME_ADDRESS(u32 x) {
    return ((x & 0xFFFFF000) >> 12);
}

template<typename T>
static inline T* PHYS_TO_VIRT(T* x) {
    return reinterpret_cast<T*>(reinterpret_cast<u32>(x) + 0xC0000000);
}

static inline u32 PHYS_TO_VIRT(u32 x) {
    return (x + 0xC0000000);
}

template<typename T>
static inline T* VIRT_2_PHYS(T* x) {
    return reinterpret_cast<T*>(reinterpret_cast<u32>(x) - 0xC0000000);
}

static inline u32 VIRT_2_PHYS(u32 x) {
    return (x - 0xC0000000);
}

static inline void invlpg(void* addr) {
    asm volatile("invlpg (%0)" ::"r"(addr) : "memory");
}

struct PageDirectoryEntry {
    u32 present : 1;
    u32 readWrite : 1;
    u32 isUser : 1;
    u32 writeThrough : 1;
    u32 canCache : 1;
    u32 accessed : 1;
    u32 reserved : 1;
    u32 pageSize : 1;
    u32 ignored : 1;
    u32 unused : 3;
    u32 frame : 20;
} __attribute__((packed));

struct PageTableEntry {
    u32 present : 1;
    u32 readWrite : 1;
    u32 isUser : 1;
    u32 writeThrough : 1;
    u32 canCache : 1;
    u32 accessed : 1;
    u32 dirty : 1;
    u32 reserved : 1;
    u32 global : 1;
    u32 unused : 3;
    u32 frame : 20;
} __attribute__((packed));

struct PageTable {
    PageTableEntry entries[1024];
} __attribute__((packed));

struct PageDirectory {
    PageDirectoryEntry entries[1024];
} __attribute__((packed));

class MemoryManager {
public:
    static MemoryManager& instance() {
        static MemoryManager memoryManager;
        return memoryManager;
    }

    MemoryManager() = default;

    void init(u32 addr);
    u32 allocBlock(u32 count);
    void printBuddyVisualization();
    void mapPage(u32 phys, u32 virt);

    void switchPageDirectory(PageDirectory* dir);

private:
    static constexpr u32 MIN_BLOCK_SIZE = 4096;
    static constexpr u32 NUM_LEVELS = 5;
    static constexpr u32 MAX_BLOCK_SIZE = MIN_BLOCK_SIZE << (NUM_LEVELS - 1);
    static constexpr u32 MAX_MEMORY_MB = 64;
    static constexpr u32 MAX_BLOCKS_PER_LEVEL = (MAX_MEMORY_MB * 1024 * 1024) / MIN_BLOCK_SIZE;
    static constexpr u32 MAX_BITMAP_SIZE = (MAX_BLOCKS_PER_LEVEL + 7) / 8;

    PageDirectory* currentPageDirectory;

    u32 totalMemory;
    void initPMM(u32 addr);
    void initVMM();

    u8 bitmapAddress[NUM_LEVELS][MAX_BITMAP_SIZE];
    Bitmap bitmaps[NUM_LEVELS];

    void initBuddyAllocator(u32 addr);
    u32 indexToLevel(u32 sourceLevel, u32 targetLevel, u32 index);
    u32 getNearestFreeblock(u32 level);

    void allocBlocks(u32 level, u32 firstBlock, u32 count);
    void markRegionAllocated(u32 addr, u32 length);
};
