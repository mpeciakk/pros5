#pragma once

#include "Lib/Types.hpp"

class Framebuffer {
    public:
        u32 width;
        u32 height;

        static Framebuffer& instance() {
            static Framebuffer framebuffer;
            return framebuffer;
        }

        Framebuffer() = default;

        void init(u32* addr, u32 width, u32 height, u32 pitch, u32 bpp);
        void putPixel(u32 x, u32 y, u32 color);
        void fillRect(u32 x, u32 y, u32 width, u32 height, u32 color);
        void clear(u32 color);
        u32 rgb(u8 r, u8 g, u8 b);
        u32 rgba(u8 r, u8 g, u8 b, u8 a);

    private:
        u32* buffer;
        u32 pitch;
        u32 bpp;
};

class FramebufferConsole {
    public:
        static FramebufferConsole& instance() {
            static FramebufferConsole framebufferConsole;
            return framebufferConsole;
        }

        FramebufferConsole() = default;

        void init();
        void putChar(char c);
        void print(const char* str);
        void printCentered(const char* str);
        void println(const char* str);
        void setColor(u32 color);

    private:
        u32 x;
        u32 y;
        u32 color;

        static const u8 font8x8[128][8];
};