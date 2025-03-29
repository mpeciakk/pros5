#include "MM/MemoryManager.hpp"
#include "Lib/Log.hpp"
#include "Lib/String.hpp"

void MemoryManager::initVMM() {
    Log::debug("[VMM] Initializing");

    auto* table = (PageTable*) phys2virt(allocBlock(1));
    memset(table, 0, sizeof(PageTable));

    auto* dir = (PageDirectory*) phys2virt(allocBlock(1));
    memset(dir, 0, sizeof(PageDirectory));

    Log::debug("VMM: Kernel page directory at %x, kernel page table at %x", (u32) dir, (u32) table);

    for (u32 i = 0, physicalAddress = 0, virtualAddress = 0xC0000000; i < 1024; i++, physicalAddress += PAGE_SIZE, virtualAddress += PAGE_SIZE) {
        PageTableEntry page{};
        page.present = true;
        page.readWrite = true;
        page.frame = GET_FRAME_ADDRESS(physicalAddress);

        table->entries[PAGE_TABLE_INDEX(virtualAddress)] = page;
    }

    auto entry = &dir->entries[PAGE_DIRECTORY_INDEX(0xC0000000)];
    entry->present = true;
    entry->readWrite = true;
    entry->frame = GET_FRAME_ADDRESS((u32) virt2phys(table));

    currentPageDirectory = (PageDirectory*) virt2phys(dir);
    asm("mov %0, %%cr3" : : "r"(virt2phys(dir)));
    asm("invlpg 0");
}

void MemoryManager::mapPage(u32 phys, u32 virt) {
    auto* pd = (PageDirectory*) ((u32) currentPageDirectory + 0xC0000000);
    auto* entry = &pd->entries[PAGE_DIRECTORY_INDEX(virt)];

    Log::debug("VMM: Mapping page %x to %x, pd: %x, entry: %x", phys, virt, (u32) pd, (u32) entry);
    
    if (!entry->present) {
        auto* table = (PageTable*) phys2virt(allocBlock(1));
        memset(table, 0, sizeof(PageTable));

        PageTableEntry page{};
        page.present = true;
        page.readWrite = true;
        page.frame = GET_FRAME_ADDRESS(phys);

        table->entries[PAGE_TABLE_INDEX(virt)] = page;

        entry->present = true;
        entry->readWrite = true;
        entry->frame = GET_FRAME_ADDRESS(virt2phys(table));

        Log::debug("VMM: Page table at %x, entry: %x", (u32) table, (u32) &table->entries[PAGE_TABLE_INDEX(virt)]);
    } else {
        auto* table = (PageTable*) phys2virt(PAGE_GET_PHYSICAL_ADDRESS((u32*) entry));
        auto* page = (PageTableEntry*) phys2virt(&table->entries[PAGE_TABLE_INDEX(virt)]);
        
        page->present = true;
        page->readWrite = true;
        page->frame = GET_FRAME_ADDRESS((u32) phys);
    }
}