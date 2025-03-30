#include "Lib/String.hpp"
#include "MM/Heap.hpp"
#include "MM/MemoryManager.hpp"
#include "MM/KernelHeap.hpp"

void* kmalloc(u32 size) {
    return KernelHeap::instance().malloc(size);
}

void free(void* address) {
    return KernelHeap::instance().free(address);
}

void* operator new(long unsigned int size) {
    return kmalloc(size);
}

void* operator new[](long unsigned int size) {
    return kmalloc(size);
}

void* operator new(long unsigned int size, void* ptr) {
    return ptr;
}

void* operator new[](long unsigned int size, void* ptr) {
    return ptr;
}

void operator delete(void* ptr) {
    free(ptr);
}

void operator delete(void* ptr, u32 size) {
    free(ptr);
}

void operator delete(void* ptr, unsigned long size) {
    free(ptr);
}

void operator delete[](void* ptr) {
    free(ptr);
}

void operator delete[](void* ptr, u32 size) {
    free(ptr);
}