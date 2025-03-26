#include "Hardware/IDT.hpp"
#include "Lib/Log.hpp"
#include "Hardware/Port.hpp"

IDTEntry InterruptManager::entries[256];
IDTPointer InterruptManager::ptr;

extern "C" void IDTFlush(u32 pointer);

static inline void outb(u16 port, u8 val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

extern "C" void handleInterrupt(Registers* regs) {
    if (regs->interrupt < 0x20) {
        Log::debug("Interrupt: %x", regs->interrupt);
    }

    outb(0x20, 0x20);
    
    if (regs->interrupt >= 40) {
        outb(0xA0, 0x20);
    }
}

InterruptManager::InterruptManager() : masterCommandPort(0x20), masterDataPort(0x21), slaveCommandPort(0xA0), slaveDataPort(0xA1) {
    masterCommandPort.write(0x11);
    slaveCommandPort.write(0x11);

    masterDataPort.write(0x20);
    slaveDataPort.write(0x28);

    masterDataPort.write(0x04);
    slaveDataPort.write(0x02);

    masterDataPort.write(0x01);
    slaveDataPort.write(0x01);

    masterDataPort.write(0x00);
    slaveDataPort.write(0x00);
}

void InterruptManager::init() {
    ptr.limit = (sizeof(IDTEntry) * 256) - 1;
    ptr.base = (u32) &entries;

    setEntry(0, (u32) isr0, 0x08, 0x8E);
    setEntry(1, (u32) isr1, 0x08, 0x8E);
    setEntry(2, (u32) isr2, 0x08, 0x8E);
    setEntry(3, (u32) isr3, 0x08, 0x8E);
    setEntry(4, (u32) isr4, 0x08, 0x8E);
    setEntry(5, (u32) isr5, 0x08, 0x8E);
    setEntry(6, (u32) isr6, 0x08, 0x8E);
    setEntry(7, (u32) isr7, 0x08, 0x8E);
    setEntry(8, (u32) isr8, 0x08, 0x8E);
    setEntry(9, (u32) isr9, 0x08, 0x8E);
    setEntry(10, (u32) isr10, 0x08, 0x8E);
    setEntry(11, (u32) isr11, 0x08, 0x8E);
    setEntry(12, (u32) isr12, 0x08, 0x8E);
    setEntry(13, (u32) isr13, 0x08, 0x8E);
    setEntry(14, (u32) isr14, 0x08, 0x8E);
    setEntry(15, (u32) isr15, 0x08, 0x8E);
    setEntry(16, (u32) isr16, 0x08, 0x8E);
    setEntry(17, (u32) isr17, 0x08, 0x8E);
    setEntry(18, (u32) isr18, 0x08, 0x8E);
    setEntry(19, (u32) isr19, 0x08, 0x8E);
    setEntry(20, (u32) isr20, 0x08, 0x8E);
    setEntry(21, (u32) isr21, 0x08, 0x8E);
    setEntry(22, (u32) isr22, 0x08, 0x8E);
    setEntry(23, (u32) isr23, 0x08, 0x8E);
    setEntry(24, (u32) isr24, 0x08, 0x8E);
    setEntry(25, (u32) isr25, 0x08, 0x8E);
    setEntry(26, (u32) isr26, 0x08, 0x8E);
    setEntry(27, (u32) isr27, 0x08, 0x8E);
    setEntry(28, (u32) isr28, 0x08, 0x8E);
    setEntry(29, (u32) isr29, 0x08, 0x8E);
    setEntry(30, (u32) isr30, 0x08, 0x8E);
    setEntry(31, (u32) isr31, 0x08, 0x8E);

    setEntry(0x20 + 0, (u32) irq0, 0x08, 0x8E);
    setEntry(0x20 + 1, (u32) irq1, 0x08, 0x8E);
    setEntry(0x20 + 2, (u32) irq2, 0x08, 0x8E);
    setEntry(0x20 + 3, (u32) irq3, 0x08, 0x8E);
    setEntry(0x20 + 4, (u32) irq4, 0x08, 0x8E);
    setEntry(0x20 + 5, (u32) irq5, 0x08, 0x8E);
    setEntry(0x20 + 6, (u32) irq6, 0x08, 0x8E);
    setEntry(0x20 + 7, (u32) irq7, 0x08, 0x8E);
    setEntry(0x20 + 8, (u32) irq8, 0x08, 0x8E);
    setEntry(0x20 + 9, (u32) irq9, 0x08, 0x8E);
    setEntry(0x20 + 10, (u32) irq10, 0x08, 0x8E);
    setEntry(0x20 + 11, (u32) irq11, 0x08, 0x8E);
    setEntry(0x20 + 12, (u32) irq12, 0x08, 0x8E);
    setEntry(0x20 + 13, (u32) irq13, 0x08, 0x8E);
    setEntry(0x20 + 14, (u32) irq14, 0x08, 0x8E);
    setEntry(0x20 + 15, (u32) irq15, 0x08, 0x8E);

    IDTFlush((u32) &ptr);
}

void InterruptManager::setEntry(u8 i, u32 base, u16 selector, u8 flags) {
    entries[i].baseLow = base & 0xFFFF;
    entries[i].baseHigh = (base >> 16) & 0xFFFF;
    entries[i].flags = flags;
    entries[i].selector = selector;
    entries[i].zero = 0;
}

void InterruptManager::enable() {
    asm volatile("sti");
}

void InterruptManager::disable() {
    asm volatile("cli");
}