#pragma once

#include "Lib/Types.hpp"

#define HEAP_MAGIC 0x2137DEAD

struct MemoryChunk {
    MemoryChunk* next;
    MemoryChunk* prev;

    u32 magic;
    bool allocated;
    u32 size;
};

class Heap {
public:
    Heap(u32 start, u32 size);

    void* malloc(u32 size);
    void free(void* ptr);
    void dump();

    u32 usage();

private:
    u32 memorySize;
    u32 usedMemory;

    MemoryChunk* first;
};
