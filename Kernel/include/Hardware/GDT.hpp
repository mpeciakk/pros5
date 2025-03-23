#pragma once
#include "Lib/Types.hpp"

struct GDTEntry {
    u16 limitLow;
    u16 baseLow;
    u8 baseMiddle;
    u8 access;
    u8 granularity;
    u8 baseHigh;
} __attribute__((packed));

struct GDTPtr {
    u16 limit;
    u32 base;
} __attribute__((packed));

class GDT {
public:
    void init();

private:
    static GDTEntry entries[6];
    static GDTPtr ptr;

    static constexpr u8 NULL_SEGMENT = 0;
    static constexpr u8 KERNEL_CODE = 1;
    static constexpr u8 KERNEL_DATA = 2;
    static constexpr u8 USER_CODE = 3;
    static constexpr u8 USER_DATA = 4;
    static constexpr u8 TSS = 5;

    void setGate(u8 num, u32 base, u32 limit, u8 access, u8 gran);
};