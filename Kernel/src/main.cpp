#include "Drivers/ATA.hpp"
#include "Drivers/Framebuffer.hpp"
#include "FS/DevFS.hpp"
#include "FS/PartitionTable.hpp"
#include "FS/VFS.hpp"
#include "Hardware/GDT.hpp"
#include "Hardware/IDT.hpp"
#include "Lib/Log.hpp"
#include "MM/KernelHeap.hpp"
#include "MM/MemoryManager.hpp"
#include "Multiboot/multiboot.h"
#include "Papiez.hpp"

void stage1(u32 addr) {
    Log::debug("[Stage 1] Initializing");

    GDT gdt;
    gdt.init();
    Log::debug("[GDT] initialized");

    InterruptManager::instance().init();
    InterruptManager::instance().enable();
    Log::debug("[InterruptManager] initialized and enabled");

    MemoryManager::instance().init(addr);
    Log::debug("[MM] initialized");

    u32 physicalAddress = MemoryManager::instance().allocBlock(1);
    for (u32 virtualAddress = KERNEL_HEAP_START; virtualAddress < KERNEL_HEAP_START + KERNEL_HEAP_SIZE;
         physicalAddress = MemoryManager::instance().allocBlock(1), virtualAddress += PAGE_SIZE) {
        MemoryManager::instance().mapPage(physicalAddress, virtualAddress);
    }

    KernelHeap::instance();
    Log::debug("[KernelHeap] initialized");

    Log::debug("[Stage 1] Initialized\n");
}

void stage2(u32 addr) {
    Log::debug("[Stage 2] Initializing");

    multiboot_tag* tag = (struct multiboot_tag*) (addr + 8);

    while (tag->type != MULTIBOOT_TAG_TYPE_END) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_CMDLINE: {
                struct multiboot_tag_string* cmdline = (struct multiboot_tag_string*) tag;
                Log::debug("[MBI] Command line = %s", cmdline->string);
                break;
            }
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME: {
                struct multiboot_tag_string* bootloader = (struct multiboot_tag_string*) tag;
                Log::debug("[MBI] Boot loader name = %s", bootloader->string);
                break;
            }
            case MULTIBOOT_TAG_TYPE_MODULE: {
                struct multiboot_tag_module* module = (struct multiboot_tag_module*) tag;
                Log::debug("[MBI] Module at %x-%x. Command line %s", module->mod_start, module->mod_end, module->cmdline);
                break;
            }
            case MULTIBOOT_TAG_TYPE_BOOTDEV: {
                struct multiboot_tag_bootdev* bootdev = (struct multiboot_tag_bootdev*) tag;
                Log::debug("[MBI] Boot device %x,%u,%u", bootdev->biosdev, bootdev->slice, bootdev->part);
                break;
            }
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
                struct multiboot_tag_framebuffer* tagfb = (struct multiboot_tag_framebuffer*) tag;
                Log::debug("[Framebuffer] info:");
                Log::debug("Address: %x", (u32) tagfb->common.framebuffer_addr);
                Log::debug("Width: %x", tagfb->common.framebuffer_width);
                Log::debug("Height: %x", tagfb->common.framebuffer_height);
                Log::debug("Pitch: %x", tagfb->common.framebuffer_pitch);
                Log::debug("BPP: %x", tagfb->common.framebuffer_bpp);
                Log::debug("Type: %x", tagfb->common.framebuffer_type);
                Log::debug("Red position: %x", tagfb->framebuffer_red_field_position);
                Log::debug("Red mask size: %x", tagfb->framebuffer_red_mask_size);
                Log::debug("Green position: %x", tagfb->framebuffer_green_field_position);
                Log::debug("Green mask size: %x", tagfb->framebuffer_green_mask_size);
                Log::debug("Blue position: %x", tagfb->framebuffer_blue_field_position);
                Log::debug("Blue mask size: %x", tagfb->framebuffer_blue_mask_size);

                if (!tagfb->common.framebuffer_addr || !tagfb->common.framebuffer_width || !tagfb->common.framebuffer_height ||
                    !tagfb->common.framebuffer_pitch || !tagfb->common.framebuffer_bpp) {
                    Log::debug("Invalid framebuffer parameters");
                    break;
                }

                if (tagfb->common.framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_RGB) {
                    Log::debug("Not RGB framebuffer");
                    break;
                }

                if (tagfb->common.framebuffer_bpp != 32) {
                    Log::debug("Not 32-bit color");
                    break;
                }

                u32 size = tagfb->common.framebuffer_width * tagfb->common.framebuffer_height * 4;

                for (u32 i = 0, physicalAddress = tagfb->common.framebuffer_addr, virtualAddress = 0xC0500000; i < size / PAGE_SIZE;
                     i++, physicalAddress += PAGE_SIZE, virtualAddress += PAGE_SIZE) {

                    MemoryManager::instance().mapPage(physicalAddress, virtualAddress);
                }

                Framebuffer::instance().init((u32*) (0xC0500000), tagfb->common.framebuffer_width, tagfb->common.framebuffer_height,
                                             tagfb->common.framebuffer_pitch, tagfb->common.framebuffer_bpp);

                Framebuffer::instance().clear(Framebuffer::instance().rgb(0, 0, 32));
                FramebufferConsole::instance().init();

                for (int i = 0; i < PAPIEZ_WIDTH; i++) {
                    for (int j = 0; j < PAPIEZ_HEIGHT; j++) {
                        Framebuffer::instance().putPixel(i + 150, j + 150, PAPIEZ[j * PAPIEZ_WIDTH + i]);
                    }
                }

                break;
            }
        }

        tag = (struct multiboot_tag*) ((multiboot_uint8_t*) tag + ((tag->size + 7) & ~7));
    }

    ATA ata0m(0x1F0, true);
    ATA ata0s(0x1F0, false);
    ata0m.identify();
    ata0s.identify();

    DevFS devfs;
    if (!VFSManager::instance().mount("/dev", &devfs)) {
        Log::error("Failed to mount /dev");
    }

    if (!devfs.registerDevice("ata0m", FileType::BlockDevice, &ata0m)) {
        Log::error("Failed to register ata0m");
    }
    if (!devfs.registerDevice("ata0s", FileType::BlockDevice, &ata0s)) {
        Log::error("Failed to register ata0s");
    }

    auto fd = VFSManager::instance().open("/dev/ata0s", 0);
    if (!fd) {
        Log::error("Failed to open /dev/ata0s");
    }

    MasterBootRecord mbr;
    u32 size = VFSManager::instance().read(fd, &mbr, sizeof(MasterBootRecord));

    if (mbr.magicNumber != 0xAA55) {
        Log::error("MBR Error");
    }

    for (int i = 0; i < 4; ++i) {
        if (mbr.primaryPartition[i].type == 0x00) {
            continue;
        }

        bool bootable = mbr.primaryPartition[i].bootable == 0x80;

        Log::log("Partition %d %s, type: %x", i, bootable ? "bootable" : "not bootable",
            mbr.primaryPartition[i].type);
    }

    VFSManager::instance().close(fd);

    Log::debug("[Stage 2] Initialized");
}

extern "C" [[noreturn]] void kmain(unsigned long magic, unsigned long addr) {
    Log::debug("[Stage 0] Initializing");

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        Log::debug("Invalid magic number: %x", magic);
        while (true) {
            __asm__ volatile("hlt");
        }
    }

    Log::debug("Magic number OK");

    if (addr & 7) {
        Log::debug("Unaligned mbi: %x", addr);
        while (true) {
            __asm__ volatile("hlt");
        }
    }

    Log::debug("MBI aligned OK");

    Log::debug("[Stage 0] Initialized\n");

    stage1(addr + 0xC0000000);
    stage2(addr + 0xC0000000);

    Log::debug("Kernel initialized");

    while (true) {
        __asm__ volatile("hlt");
    }
}
