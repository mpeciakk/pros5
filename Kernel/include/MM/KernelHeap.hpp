#include "Lib/Log.hpp"
#include "Lib/Types.hpp"
#include "MM/Heap.hpp"

#define KERNEL_HEAP_START 0xC0400000
#define KERNEL_HEAP_SIZE (1024 * 1024)

class KernelHeap : public Heap {
public:
    KernelHeap() : Heap(KERNEL_HEAP_START, KERNEL_HEAP_SIZE) {
        Log::debug("KernelHeap: Initializing on %x", KERNEL_HEAP_START);
    }

    static KernelHeap& instance() {
        static KernelHeap heap;
        return heap;
    }
};

void* kmalloc(u32 size);
void free(void* address);
