/*
 * ps1-bare-metal - (C) 2023-2025 spicyjpeg
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

// NOTE: 4096 words (16KB per chain, 32KB total for double-buffer). Increased
// from 1024 to support heavy text rendering (e.g. 8 controllers on screen).
constexpr const int iPSXDMAListSize      = 4096;
constexpr const uint8_t bPSXDMAChunkSize = 16;

/* DMA tags */

static inline uint32_t __attribute__((always_inline))
gp0_tag(size_t length, void *next)
{
    return 0 | (((uint32_t)next & 0xffffff) << 0) | (((uint32_t)length & 0x0000ff) << 24);
}

static inline uint32_t __attribute__((always_inline))
gp0_endTag(size_t length)
{
    return gp0_tag(length, (void *)0xffffff);
}

/* Drawing attributes */

enum GP0BlendMode : uint32_t
{
    GP0_BLEND_BITMASK   = 3,
    GP0_BLEND_SEMITRANS = 0,
    GP0_BLEND_ADD       = 1,
    GP0_BLEND_SUBTRACT  = 2,
    GP0_BLEND_DIV4_ADD  = 3
};

enum GP0ColorDepth : uint32_t
{
    GP0_COLOR_BITMASK = 3,
    GP0_COLOR_4BPP    = 0,
    GP0_COLOR_8BPP    = 1,
    GP0_COLOR_16BPP   = 2
};

constexpr uint16_t __attribute__((always_inline))
gp0_page(
    unsigned int x,
    unsigned int y,
    GP0BlendMode blendMode,
    GP0ColorDepth colorDepth)
{
    return 0 | ((x & 15) << 0) | ((y & 1) << 4) | ((blendMode & 3) << 5) | ((colorDepth & 3) << 7) | ((y & 2) << 10);
}
constexpr uint16_t __attribute__((always_inline))
gp0_clut(unsigned int x, unsigned int y)
{
    return 0 | ((x & 0x03f) << 0) | ((y & 0x3ff) << 6);
}

constexpr uint32_t __attribute__((always_inline))
gp0_xy(int x, int y)
{
    return 0 | ((x & 0xffff) << 0) | ((y & 0xffff) << 16);
}
constexpr uint32_t __attribute__((always_inline))
gp0_uv(unsigned int u, unsigned int v, uint16_t attr)
{
    return 0 | ((u & 0x00ff) << 0) | ((v & 0x00ff) << 8) | ((attr & 0xffff) << 16);
}
constexpr uint32_t __attribute__((always_inline))
gp0_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return 0 | ((r & 0xff) << 0) | ((g & 0xff) << 8) | ((b & 0xff) << 16);
}

/* GP0 (drawing) commands */

enum GP0Command : uint32_t
{
    GP0_CMD_MISC       = 0u << 29,
    GP0_CMD_POLYGON    = 1u << 29,
    GP0_CMD_LINE       = 2u << 29,
    GP0_CMD_RECTANGLE  = 3u << 29,
    GP0_CMD_VRAM_BLIT  = 4u << 29,
    GP0_CMD_VRAM_WRITE = 5u << 29,
    GP0_CMD_VRAM_READ  = 6u << 29,
    GP0_CMD_ATTRIBUTE  = 7u << 29
};

enum GP0MiscCommand : uint32_t
{
    GP0_CMD_NOP         = GP0_CMD_MISC | (0 << 24),
    GP0_CMD_FLUSH_CACHE = GP0_CMD_MISC | (1 << 24),
    GP0_CMD_VRAM_FILL   = GP0_CMD_MISC | (2 << 24),
    GP0_CMD_NOP2        = GP0_CMD_MISC | (3 << 24),
    GP0_CMD_IRQ         = GP0_CMD_MISC | (31 << 24)
};

enum GP0AttributeCommand : uint32_t
{
    GP0_CMD_TEXPAGE    = GP0_CMD_ATTRIBUTE | (1 << 24),
    GP0_CMD_TEXWINDOW  = GP0_CMD_ATTRIBUTE | (2 << 24),
    GP0_CMD_FB_OFFSET1 = GP0_CMD_ATTRIBUTE | (3 << 24),
    GP0_CMD_FB_OFFSET2 = GP0_CMD_ATTRIBUTE | (4 << 24),
    GP0_CMD_FB_ORIGIN  = GP0_CMD_ATTRIBUTE | (5 << 24),
    GP0_CMD_FB_MASK    = GP0_CMD_ATTRIBUTE | (6 << 24)
};

constexpr uint32_t __attribute__((always_inline))
_gp0_polygon(
    bool quad,
    bool unshaded,
    bool gouraud,
    bool textured,
    bool blend)
{
    return GP0_CMD_POLYGON | ((unshaded & 1) << 24) | ((blend & 1) << 25) | ((textured & 1) << 26) | ((quad & 1) << 27) | ((gouraud & 1) << 28);
}
constexpr uint32_t __attribute__((always_inline))
gp0_triangle(bool textured, bool blend)
{
    return _gp0_polygon(false, true, false, textured, blend);
}
constexpr uint32_t __attribute__((always_inline))
gp0_shadedTriangle(bool gouraud, bool textured, bool blend)
{
    return _gp0_polygon(false, false, gouraud, textured, blend);
}
constexpr uint32_t __attribute__((always_inline))
gp0_quad(bool textured, bool blend)
{
    return _gp0_polygon(true, true, false, textured, blend);
}
constexpr uint32_t __attribute__((always_inline))
gp0_shadedQuad(bool gouraud, bool textured, bool blend)
{
    return _gp0_polygon(true, false, gouraud, textured, blend);
}

constexpr uint32_t __attribute__((always_inline))
gp0_line(bool gouraud, bool blend)
{
    return GP0_CMD_LINE | ((blend & 1) << 25) | ((gouraud & 1) << 28);
}
constexpr uint32_t __attribute__((always_inline))
gp0_polyLine(bool gouraud, bool blend)
{
    return GP0_CMD_LINE | ((blend & 1) << 25) | (1 << 27) | ((gouraud & 1) << 28);
}

constexpr uint32_t __attribute__((always_inline))
_gp0_rectangle(
    uint8_t size,
    bool textured,
    bool unshaded,
    bool blend)
{
    return GP0_CMD_RECTANGLE | ((unshaded & 1) << 24) | ((blend & 1) << 25) | ((textured & 1) << 26) | ((size & 3) << 27);
}
constexpr uint32_t __attribute__((always_inline))
gp0_rectangle(bool textured, bool unshaded, bool blend)
{
    return _gp0_rectangle(0, textured, unshaded, blend);
}
constexpr uint32_t __attribute__((always_inline))
gp0_rectangle1x1(bool textured, bool unshaded, bool blend)
{
    return _gp0_rectangle(1, textured, unshaded, blend);
}
constexpr uint32_t __attribute__((always_inline))
gp0_rectangle8x8(bool textured, bool unshaded, bool blend)
{
    return _gp0_rectangle(2, textured, unshaded, blend);
}
constexpr uint32_t __attribute__((always_inline))
gp0_rectangle16x16(bool textured, bool unshaded, bool blend)
{
    return _gp0_rectangle(3, textured, unshaded, blend);
}

constexpr uint32_t __attribute__((always_inline))
gp0_vramBlit(void)
{
    return GP0_CMD_VRAM_BLIT;
}
constexpr uint32_t __attribute__((always_inline))
gp0_vramWrite(void)
{
    return GP0_CMD_VRAM_WRITE;
}
constexpr uint32_t __attribute__((always_inline))
gp0_vramRead(void)
{
    return GP0_CMD_VRAM_READ;
}

constexpr uint32_t __attribute__((always_inline))
gp0_flushCache(void)
{
    return GP0_CMD_FLUSH_CACHE;
}
constexpr uint32_t __attribute__((always_inline))
gp0_vramFill(void)
{
    return GP0_CMD_VRAM_FILL;
}
constexpr uint32_t __attribute__((always_inline))
gp0_irq(void)
{
    return GP0_CMD_IRQ;
}

constexpr uint32_t __attribute__((always_inline))
gp0_texpage(uint16_t page, bool dither, bool unlockFB)
{
    return GP0_CMD_TEXPAGE | ((page & 0x9ff) << 0) | ((dither & 1) << 9) | ((unlockFB & 1) << 10);
}
constexpr uint32_t __attribute__((always_inline))
gp0_texwindow(
    uint8_t baseX,
    uint8_t baseY,
    uint8_t maskX,
    uint8_t maskY)
{
    return GP0_CMD_TEXWINDOW | ((maskX & 0x1f) << 0) | ((maskY & 0x1f) << 5) | ((baseX & 0x1f) << 10) | ((baseY & 0x1f) << 15);
}
constexpr uint32_t __attribute__((always_inline))
gp0_fbOffset1(unsigned int x, unsigned int y)
{
    return GP0_CMD_FB_OFFSET1 | ((x & 0x3ff) << 0) | ((y & 0x3ff) << 10);
}
constexpr uint32_t __attribute__((always_inline))
gp0_fbOffset2(unsigned int x, unsigned int y)
{
    return GP0_CMD_FB_OFFSET2 | ((x & 0x3ff) << 0) | ((y & 0x3ff) << 10);
}
constexpr uint32_t __attribute__((always_inline))
gp0_fbOrigin(int x, int y)
{
    return GP0_CMD_FB_ORIGIN | ((x & 0x7ff) << 0) | ((y & 0x7ff) << 11);
}
constexpr uint32_t __attribute__((always_inline))
gp0_fbMask(bool setMask, bool useMask)
{
    return GP0_CMD_FB_MASK | (setMask << 0) | (useMask << 1);
}

/* GP1 (display control) commands */

enum GP1HorizontalRes : uint32_t
{
    GP1_HRES_BITMASK = (3 << 0) | (1 << 6),
    GP1_HRES_256     = 0 << 0, // Dotclock divided by 10
    GP1_HRES_320     = 1 << 0, // Dotclock divided by 8
    GP1_HRES_368     = 1 << 6, // Dotclock divided by 7
    GP1_HRES_512     = 2 << 0, // Dotclock divided by 5
    GP1_HRES_640     = 3 << 0  // Dotclock divided by 4
};

constexpr GP1HorizontalRes GP1HorizontalResList[5] = {
    GP1_HRES_256,
    GP1_HRES_320,
    GP1_HRES_368,
    GP1_HRES_512,
    GP1_HRES_640};

constexpr uint16_t GPURawHorizontalResolution[5] = {
    256,
    320,
    368,
    512,
    640};

enum GP1VerticalRes : uint32_t
{
    GP1_VRES_BITMASK = 1,
    GP1_VRES_256     = 0,
    GP1_VRES_512     = 1
};

constexpr GP1VerticalRes GP1VerticalResList[2] = {
    GP1_VRES_256,
    GP1_VRES_512};

constexpr uint16_t GPURawVerticalResolution[2] = {
    256,
    512};

enum GP1VideoMode : uint32_t
{
    GP1_MODE_BITMASK = 1,
    GP1_MODE_NTSC    = 0,
    GP1_MODE_PAL     = 1
};

enum GP1ColorDepth : uint32_t
{
    GP1_COLOR_BITMASK = 1,
    GP1_COLOR_16BPP   = 0,
    GP1_COLOR_24BPP   = 1
};

enum GP1DMARequestMode : uint32_t
{
    GP1_DREQ_BITMASK   = 3,
    GP1_DREQ_NONE      = 0,
    GP1_DREQ_FIFO      = 1,
    GP1_DREQ_GP0_WRITE = 2,
    GP1_DREQ_GP0_READ  = 3
};

enum GP1VRAMSize : uint32_t
{
    GP1_VRAM_BITMASK = 1,
    GP1_VRAM_1MB     = 0,
    GP1_VRAM_2MB     = 1
};

enum GP1Command : uint32_t
{
    GP1_CMD_RESET_GPU   = 0 << 24,
    GP1_CMD_RESET_FIFO  = 1 << 24,
    GP1_CMD_ACKNOWLEDGE = 2 << 24,
    GP1_CMD_DISP_BLANK  = 3 << 24,
    GP1_CMD_DREQ_MODE   = 4 << 24,
    GP1_CMD_FB_OFFSET   = 5 << 24,
    GP1_CMD_FB_RANGE_H  = 6 << 24,
    GP1_CMD_FB_RANGE_V  = 7 << 24,
    GP1_CMD_FB_MODE     = 8 << 24,
    GP1_CMD_VRAM_SIZE   = 9 << 24,
    GP1_CMD_GET_INFO    = 16 << 24
};

constexpr uint32_t __attribute__((always_inline))
gp1_clockMultiplierH(GP1HorizontalRes horizontalRes)
{
    switch (horizontalRes)
    {
    case GP1_HRES_256:
        return 10;
    case GP1_HRES_320:
        return 8;
    case GP1_HRES_368:
        return 7;
    case GP1_HRES_512:
        return 5;
    case GP1_HRES_640:
        return 4;
    default:
        return 0;
    }
}

constexpr uint32_t __attribute__((always_inline))
gp1_clockDividerV(GP1VerticalRes verticalRes)
{
    switch (verticalRes)
    {
    case GP1_VRES_256:
        return 1;
    case GP1_VRES_512:
        return 2;
    default:
        return 0;
    }
}

constexpr uint32_t __attribute__((always_inline))
gp1_resetGPU(void)
{
    return GP1_CMD_RESET_GPU;
}
constexpr uint32_t __attribute__((always_inline))
gp1_resetFIFO(void)
{
    return GP1_CMD_RESET_FIFO;
}
constexpr uint32_t __attribute__((always_inline))
gp1_acknowledge(void)
{
    return GP1_CMD_ACKNOWLEDGE;
}
constexpr uint32_t __attribute__((always_inline))
gp1_dispBlank(bool blank)
{
    return GP1_CMD_DISP_BLANK | (blank & 1);
}
constexpr uint32_t __attribute__((always_inline))
gp1_dmaRequestMode(GP1DMARequestMode mode)
{
    return GP1_CMD_DREQ_MODE | (mode & 3);
}
constexpr uint32_t __attribute__((always_inline))
gp1_fbOffset(unsigned int x, unsigned int y)
{
    return GP1_CMD_FB_OFFSET | ((x & 0x3ff) << 0) | ((y & 0x3ff) << 10);
}
constexpr uint32_t __attribute__((always_inline))
gp1_fbRangeH(unsigned int low, unsigned int high)
{
    return GP1_CMD_FB_RANGE_H | ((low & 0xfff) << 0) | ((high & 0xfff) << 12);
}
constexpr uint32_t __attribute__((always_inline))
gp1_fbRangeV(unsigned int low, unsigned int high)
{
    return GP1_CMD_FB_RANGE_V | ((low & 0x3ff) << 0) | ((high & 0x3ff) << 10);
}
constexpr uint32_t __attribute__((always_inline))
gp1_fbMode(
    GP1HorizontalRes horizontalRes,
    GP1VerticalRes verticalRes,
    GP1VideoMode videoMode,
    bool interlace,
    GP1ColorDepth colorDepth)
{
    return GP1_CMD_FB_MODE | ((horizontalRes & 0x47) << 0) | ((verticalRes & 1) << 2) | ((videoMode & 1) << 3) | ((colorDepth & 1) << 4) | ((interlace & 1) << 5);
}
constexpr uint32_t __attribute__((always_inline))
gp1_vramSize(GP1VRAMSize size)
{
    return GP1_CMD_VRAM_SIZE | (size & 1);
}
