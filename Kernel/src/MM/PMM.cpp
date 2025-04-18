#include "Lib/Bitmap.hpp"
#include "Lib/Log.hpp"
#include "Lib/String.hpp"
#include "MM/MemoryManager.hpp"

void MemoryManager::initPMM(u32 addr) {
    multiboot_tag* tag = (struct multiboot_tag*) (addr + 8);

    while (tag->type != MULTIBOOT_TAG_TYPE_END) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO: {
                struct multiboot_tag_basic_meminfo* meminfo = (struct multiboot_tag_basic_meminfo*) tag;
                Log::debug("[MM] mem_lower = %uKB, mem_upper = %uKB", meminfo->mem_lower, meminfo->mem_upper);

                totalMemory = meminfo->mem_upper;
                break;
            }
        }
        tag = (struct multiboot_tag*) ((multiboot_uint8_t*) tag + ((tag->size + 7) & ~7));
    }

    initBuddyAllocator(addr);

    tag = (struct multiboot_tag*) (addr + 8);
    while (tag->type != MULTIBOOT_TAG_TYPE_END) {
        if (tag->type == MULTIBOOT_TAG_TYPE_MMAP) {
            struct multiboot_tag_mmap* mmap = (struct multiboot_tag_mmap*) tag;
            for (multiboot_memory_map_t* entry = mmap->entries; (multiboot_uint8_t*) entry < (multiboot_uint8_t*) tag + tag->size;
                 entry = (multiboot_memory_map_t*) ((multiboot_uint8_t*) entry + mmap->entry_size)) {

                // We are in 32-bit mode so we can assume that the address and length are in the lower 32 bits
                u32 region_addr = entry->addr & 0xffffffff;
                u32 region_len = entry->len & 0xffffffff;
                u32 type = entry->type;

                const char* type_str;
                switch (type) {
                    case MULTIBOOT_MEMORY_AVAILABLE:
                        type_str = "AVAILABLE";
                        break;
                    case MULTIBOOT_MEMORY_RESERVED:
                        type_str = "RESERVED";
                        markRegionAllocated(region_addr, region_len);
                        break;
                    case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
                        type_str = "ACPI RECLAIMABLE";
                        markRegionAllocated(region_addr, region_len);
                        break;
                    case MULTIBOOT_MEMORY_NVS:
                        type_str = "NVS";
                        markRegionAllocated(region_addr, region_len);
                        break;
                    case MULTIBOOT_MEMORY_BADRAM:
                        type_str = "BADRAM";
                        markRegionAllocated(region_addr, region_len);
                        break;
                    default:
                        type_str = "UNKNOWN";
                        markRegionAllocated(region_addr, region_len);
                        break;
                }

                Log::debug("[MM] Memory region: addr = %x, length = %x, type = %x (%s)", region_addr, region_len, type, type_str);
            }
        }
        tag = (struct multiboot_tag*) ((multiboot_uint8_t*) tag + ((tag->size + 7) & ~7));
    }
}

void MemoryManager::initBuddyAllocator(u32 addr) {
    for (u32 level = 0; level < NUM_LEVELS; level++) {
        u32 blockSize = MIN_BLOCK_SIZE << level;

        u32 numBlocks = (totalMemory * 1024) / blockSize;

        bitmaps[level].size = (numBlocks + 7) / 8;
        bitmaps[level].bitmap = bitmapAddress[level];
        bitmaps[level].clear();

        Log::debug("[MM Buddy Allocator] Level %u: block_size=%uKB, num_blocks=%u, bitmap_size=%u bytes", level, blockSize / 1024,
                   numBlocks, bitmaps[level].size);
    }
}

u32 MemoryManager::getNearestFreeblock(u32 level) {
    u32 numBlocks = bitmaps[level].size * 8;

    for (u32 i = 0; i < numBlocks; i++) {
        if (!bitmaps[level].getBit(i)) {
            return i;
        }
    }

    return 0xFFFFFFFF;
}

u32 MemoryManager::indexToLevel(u32 sourceLevel, u32 targetLevel, u32 index) {
    if (targetLevel > sourceLevel) {
        return index >> (targetLevel - sourceLevel);
    } else if (targetLevel < sourceLevel) {
        return index << (sourceLevel - targetLevel);
    } else {
        return index;
    }
}

u32 MemoryManager::allocBlock(u32 count) {
    u32 bytes = count * MIN_BLOCK_SIZE;

    u32 k = bytes;
    k--;
    k |= k >> 1;
    k |= k >> 2;
    k |= k >> 4;
    k |= k >> 8;
    k |= k >> 16;
    k++;

    u32 level = 0;
    u32 blockSize = MIN_BLOCK_SIZE;
    while (blockSize < k && level < NUM_LEVELS - 1) {
        blockSize <<= 1;
        level++;
    }

    u32 m = getNearestFreeblock(level);
    if (m == 0xFFFFFFFF) {
        Log::debug("[MM] No free blocks found at level %u for %u pages", level, count);
        return 0xFFFFFFFF;
    }

    bitmaps[level].setBit(m);

    // Mark all parent blocks as allocated (up the tree)
    for (u32 i = level + 1; i < NUM_LEVELS; i++) {
        u32 parentIndex = indexToLevel(level, i, m);
        bitmaps[i].setBit(parentIndex);
    }

    // Mark all child blocks as allocated (down the tree)
    for (u32 i = level - 1; i < NUM_LEVELS; i--) {
        u32 childIndex = indexToLevel(level, i, m);

        for (u32 j = 0; j < (4 * count + (i + 1) * 4 - 1) / ((i + 1) * 4); j++) {
            bitmaps[i].setBit(childIndex + j);
        }
    }

    Log::debug("[MM] Allocated %u pages (%u bytes) at level %u, block index %u", count, blockSize, level, m);

    return m * (MIN_BLOCK_SIZE << level);
}

void MemoryManager::allocBlocks(u32 level, u32 firstBlock, u32 count) {
    for (u32 i = 0; i < count; i++) {
        u32 m = firstBlock + i;
        bitmaps[level].setBit(m);

        for (u32 i = 1; i < NUM_LEVELS; i++) {
            u32 parentIndex = indexToLevel(level, i, m);
            bitmaps[i].setBit(parentIndex);
        }

        for (u32 i = level - 1; i < NUM_LEVELS; i--) {
            u32 childIndex = indexToLevel(level, i, m);

            for (u32 j = 0; j < i * 2; j++) {
                bitmaps[i].setBit(childIndex + j);
            }
        }
    }
}

void MemoryManager::markRegionAllocated(u32 addr, u32 length) {
    u32 startBlock = addr / MIN_BLOCK_SIZE;
    u32 numBlocks = (length + MIN_BLOCK_SIZE - 1) / MIN_BLOCK_SIZE;

    allocBlocks(0, startBlock, numBlocks);
}

void MemoryManager::printBuddyVisualization() {
    Log::debug("\n[MM Buddy Allocator] Memory Visualization:");

    Log::kprint('\n');
    for (u32 s = 0; s < totalMemory + 8; s++) {
        Log::kprint('-');
    }
    Log::kprint('\n');

    for (u32 level = 0; level < NUM_LEVELS; level++) {
        u32 blockSize = MIN_BLOCK_SIZE << level;
        u32 totalBlocks = (totalMemory * 1024) / blockSize;

        if (blockSize < 10240) {
            Log::kprint(' ');
        }
        char buf[16];
        int len = itos(blockSize / 1024, buf, 10);
        for (u32 i = 0; i < len; i++) {
            Log::kprint(buf[i]);
        }
        Log::kprints("KB |");

        u32 spacing = (1 << (level + 1)) - 1;

        for (u32 i = 0; i < totalBlocks; i++) {
            for (u32 s = 0; s < spacing; s++) {
                Log::kprint(' ');
            }

            Log::kprint(bitmaps[level].getBit(i) ? '#' : '.');

            for (u32 s = 0; s < spacing; s++) {
                Log::kprint(' ');
            }

            Log::kprint('|');
        }

        Log::kprint('\n');
        for (u32 s = 0; s < totalMemory + 6; s++) {
            Log::kprint('-');
        }
        Log::kprint('\n');
    }

    Log::debug("# = Allocated, . = Free\n");
}