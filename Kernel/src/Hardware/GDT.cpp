#include "Hardware/GDT.hpp"

GDTEntry GDT::entries[6];
GDTPointer GDT::ptr;

extern "C" void GDTFlush(GDTPointer* ptr);

void GDT::setGate(u8 num, u32 base, u32 limit, u8 access, u8 gran) {
    entries[num].baseLow = (base & 0xFFFF);
    entries[num].baseMiddle = (base >> 16) & 0xFF;
    entries[num].baseHigh = (base >> 24) & 0xFF;

    entries[num].limitLow = (limit & 0xFFFF);
    entries[num].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);

    entries[num].access = access;
}

void GDT::init() {
    ptr.limit = (sizeof(GDTEntry) * 6) - 1;
    ptr.base = (u32) &entries;

    setGate(NULL_SEGMENT, 0, 0, 0, 0);
    setGate(KERNEL_CODE, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    setGate(KERNEL_DATA, 0, 0xFFFFFFFF, 0x92, 0xCF);
    setGate(USER_CODE, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    setGate(USER_DATA, 0, 0xFFFFFFFF, 0xF2, 0xCF);
    // setGate(TSS, 0, 0, 0, 0);

    GDTFlush(&ptr);
}
