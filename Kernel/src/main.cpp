#include "Drivers/Framebuffer.hpp"
#include "Hardware/GDT.hpp"
#include "Lib/Log.hpp"
#include "Multiboot/multiboot.h"
#include "Papiez.hpp"

void stage1(u32 addr) {
    Log::debug("[Stage 1] Initializing");

    GDT gdt;
    gdt.init();

    Log::debug("GDT initialized");

    Log::debug("[Stage 1] Initialized\n");
}

void stage2(u32 addr) {
    Log::debug("[Stage 2] Initializing");

    struct multiboot_tag* tag;
    unsigned size = *(unsigned*) addr;
    Log::debug("Total MBI size: %x", size);
    for (tag = (struct multiboot_tag*) (addr + 8); tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag*) ((multiboot_uint8_t*) tag + ((tag->size + 7) & ~7))) {

        Log::debug("Tag type: %x", tag->type);

        if (tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER) {
            struct multiboot_tag_framebuffer* tagfb = (struct multiboot_tag_framebuffer*) tag;

            Log::debug("Framebuffer info:");
            Log::debug("Address: %d", (u32) tagfb->common.framebuffer_addr);
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

            Framebuffer::instance().init((u32*) tagfb->common.framebuffer_addr, tagfb->common.framebuffer_width,
                                         tagfb->common.framebuffer_height, tagfb->common.framebuffer_pitch,
                                         tagfb->common.framebuffer_bpp);

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

    stage1(addr);
    stage2(addr);

    while (true) {
        __asm__ volatile("hlt");
    }
}
