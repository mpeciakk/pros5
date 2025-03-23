#include "Multiboot/multiboot.h"
#include "Drivers/Framebuffer.hpp"
#include "Papiez.hpp"
#include "Lib/Log.hpp"
#include "Hardware/GDT.hpp"

void stage1(u32 addr) {
    Log::debug("[Stage 1] Initializing");

    GDT gdt;
    gdt.init();

    Log::debug("GDT initialized");

    Log::debug("[Stage 1] Initialized\n");
}

void stage2(u32 addr) {
    Log::debug("[Stage 2] Initializing");

    struct multiboot_tag *tag;
    unsigned size = *(unsigned *)addr;
    Log::debug("Total MBI size: %x", size);
    for (tag = (struct multiboot_tag *) (addr + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7))) {
        
        Log::debug("Tag type: %x", tag->type);

        if (tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER) {
            struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *) tag;
            
            // Print framebuffer info
            Log::log("Framebuffer info:\n");
            Log::log("Address: %x", tagfb->common.framebuffer_addr);
            Log::log("\nWidth: %x", tagfb->common.framebuffer_width);
            Log::log("\nHeight: %x", tagfb->common.framebuffer_height);
            Log::log("\nPitch: %x", tagfb->common.framebuffer_pitch);
            Log::log("\nBPP: %x", tagfb->common.framebuffer_bpp);
            Log::log("\nType: %x", tagfb->common.framebuffer_type);
            Log::log("\nRed position: %x", tagfb->framebuffer_red_field_position);
            Log::log("\nRed mask size: %x", tagfb->framebuffer_red_mask_size);
            Log::log("\nGreen position: %x", tagfb->framebuffer_green_field_position);
            Log::log("\nGreen mask size: %x", tagfb->framebuffer_green_mask_size);
            Log::log("\nBlue position: %x", tagfb->framebuffer_blue_field_position);
            Log::log("\nBlue mask size: %x", tagfb->framebuffer_blue_mask_size);
            Log::log("\n");
            
            // Basic validation
            if (!tagfb->common.framebuffer_addr || 
                !tagfb->common.framebuffer_width || 
                !tagfb->common.framebuffer_height || 
                !tagfb->common.framebuffer_pitch || 
                !tagfb->common.framebuffer_bpp) {
                Log::log("Invalid framebuffer parameters");
                break;
            }

            // Only handle RGB framebuffer for now
            if (tagfb->common.framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_RGB) {
                Log::log("Not RGB framebuffer");
                break;
            }

            // Only handle 32-bit color for now
            if (tagfb->common.framebuffer_bpp != 32) {
                Log::log("Not 32-bit color");
                break;
            }

            Framebuffer::instance().init((u32*)tagfb->common.framebuffer_addr, tagfb->common.framebuffer_width, tagfb->common.framebuffer_height, tagfb->common.framebuffer_pitch, tagfb->common.framebuffer_bpp);
            // Clear screen to dark blue
            Framebuffer::instance().clear(Framebuffer::instance().rgb(0, 0, 32));
            FramebufferConsole::instance().init();

            // Draw colored rectangles
            // framebuffer.fillRect(0, 0, 100, 100, framebuffer.rgb(255, 0, 0));
            // framebuffer.fillRect(100, 0, 100, 100, framebuffer.rgb(0, 255, 0));
            // framebuffer.fillRect(200, 0, 100, 100, framebuffer.rgb(0, 0, 255));
            // framebuffer.fillRect(300, 0, 100, 100, framebuffer.rgb(255, 255, 0));
            // framebuffer.fillRect(400, 0, 100, 100, framebuffer.rgb(0, 255, 255));
            // framebuffer.fillRect(500, 0, 100, 100, framebuffer.rgb(255, 0, 255));
            
            // Draw the Papiez image
            // for (int i = 0; i < PAPIEZ_WIDTH; i++) {
            //     for (int j = 0; j < PAPIEZ_HEIGHT; j++) {
            //         framebuffer.putPixel(i + 150, j + 150, PAPIEZ[j * PAPIEZ_WIDTH + i]);
            //     }
            // }

            // // Test text rendering
            // framebuffer.print(50, 300, "Hello from the kernel!", framebuffer.rgb(255, 255, 255));  // White text
            // framebuffer.print(50, 320, "Framebuffer initialized", framebuffer.rgb(255, 0, 0));      // Red text
            
            break;
        }
    }

    Log::debug("[Stage 2] Initialized");
}

extern "C" [[noreturn]] void kmain(unsigned long magic, unsigned long addr) {
    Log::debug("[Stage 0] Initializing");

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        Log::debug("Invalid magic number: %x", magic);
        while (true) { __asm__ volatile("hlt"); }
    }

    Log::debug("Magic number OK");

    if (addr & 7) {
        Log::debug("Unaligned mbi: %x", addr);
        while (true) { __asm__ volatile("hlt"); }
    }

    Log::debug("MBI aligned OK");

    Log::debug("[Stage 0] Initialized\n");

    stage1(addr);
    stage2(addr);

    while (true) {
        __asm__ volatile("hlt");
    }
}
