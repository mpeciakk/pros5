#pragma once

#include "Lib/Types.hpp"
#include "Hardware/Port.hpp"

struct Registers {
    u32 gs, fs, es, ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 interrupt, errorCode;
    u32 eip, cs, eflags, useresp, ss;
};

struct IDTEntry {
    u16 baseLow;
    u16 selector;
    u8 zero;
    u8 flags;
    u16 baseHigh;
} __attribute__((packed));

struct IDTPointer {
    u16 limit;
    u32 base;
} __attribute__((packed));

extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();
extern "C" void irq0();
extern "C" void irq1();
extern "C" void irq2();
extern "C" void irq3();
extern "C" void irq4();
extern "C" void irq5();
extern "C" void irq6();
extern "C" void irq7();
extern "C" void irq8();
extern "C" void irq9();
extern "C" void irq10();
extern "C" void irq11();
extern "C" void irq12();
extern "C" void irq13();
extern "C" void irq14();
extern "C" void irq15();

class InterruptManager {
    public:
        static InterruptManager& instance() {
            static InterruptManager instance;
            return instance;
        }



        void init();
        void handleInterrupt(Registers registers);
        void enable();
        void disable();
    private:
        InterruptManager();

        static __attribute__((aligned(0x10))) IDTEntry entries[256];
        static IDTPointer ptr;

        Port8Bit masterCommandPort;
        Port8Bit masterDataPort;
        Port8Bit slaveCommandPort;
        Port8Bit slaveDataPort;

        void setEntry(u8 i, u32 base, u16 selector, u8 flags);
};