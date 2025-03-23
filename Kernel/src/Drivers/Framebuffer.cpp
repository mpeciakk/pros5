#include "Drivers/Framebuffer.hpp"
#include "Lib/Types.hpp"

void Framebuffer::init(u32* addr, u32 w, u32 h, u32 p, u32 bpp) {
    buffer = addr;
    width = w;
    height = h;
    pitch = p;
    bpp = bpp;
}

void Framebuffer::putPixel(u32 x, u32 y, u32 color) {
    if (x >= width || y >= height)
        return;

    u32 offset = y * (pitch / 4) + x;
    buffer[offset] = color;
}

void Framebuffer::fillRect(u32 x, u32 y, u32 w, u32 h, u32 color) {
    if (x >= width || y >= height)
        return;
    if (x + w > width)
        w = width - x;
    if (y + h > height)
        h = height - y;

    u32 offset = y * (pitch / 4) + x;

    for (u32 i = 0; i < h; i++) {
        for (u32 j = 0; j < w; j++) {
            buffer[offset + j] = color;
        }
        offset += pitch / 4;
    }
}

void Framebuffer::clear(u32 color) {
    u32 total_pixels = width * height;

    for (u32 i = 0; i < total_pixels; i++) {
        buffer[i] = color;
    }
}

u32 Framebuffer::rgb(u8 r, u8 g, u8 b) {
    return (r << 16) | (g << 8) | b;
}

u32 Framebuffer::rgba(u8 r, u8 g, u8 b, u8 a) {
    return (a << 24) | (r << 16) | (g << 8) | b;
}