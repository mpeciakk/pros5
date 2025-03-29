#include "MM/MemoryManager.hpp"
#include "Lib/Log.hpp"

void MemoryManager::init(u32 addr) {
    initPMM(addr);
}