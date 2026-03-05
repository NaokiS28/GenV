/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * halt.c - Created on 01-12-2025
 *
 * GenV is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * GenV is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * GenV. If not, see <https://www.gnu.org/licenses/>.
 */

#include "halt.h"
#include "ps1/registers.h"
#include "ps1/gpucmd.h"
#include <stddef.h>

#define DMA_MAX_CHUNK_SIZE 16

/* -------------------------------------------------------------------------
 * Font spritesheet
 *
 * The font is a 96x56 4bpp indexed spritesheet covering ASCII 0x21-0x7f,
 * identical to the one used by the ps1-bare-metal font example. The raw
 * pixel data and 16-colour palette must be linked into the binary; see the
 * CMakeLists.txt in this directory for how to convert and embed them.
 * ------------------------------------------------------------------------- */

extern const uint8_t _halt_font_image[];
extern const uint8_t _halt_font_palette[];

#define FONT_IMAGE_W 96
#define FONT_IMAGE_H 56
#define FONT_COLOR_DEPTH GP0_COLOR_4BPP
#define FONT_NUM_COLORS 16

// VRAM placement: put the font to the right of the single 320x240 framebuffer
// so it doesn't overlap with the displayed picture.
#define FONT_VRAM_X 640
#define FONT_VRAM_Y 0
#define FONT_PALETTE_X 640
#define FONT_PALETTE_Y (FONT_IMAGE_H)

#define SCREEN_W 320
#define SCREEN_H 240

/* -------------------------------------------------------------------------
 * Glyph table (ASCII 0x21 '!' through 0x7f, matching the spritesheet layout)
 * Taken directly from the ps1-bare-metal font example.
 * ------------------------------------------------------------------------- */

typedef struct
{
    uint8_t x, y, width, height;
} HaltGlyph;

static const HaltGlyph fontGlyphs[] = {
    {.x = 6, .y = 0, .width = 2, .height = 9},   // !
    {.x = 12, .y = 0, .width = 4, .height = 9},  // "
    {.x = 18, .y = 0, .width = 6, .height = 9},  // #
    {.x = 24, .y = 0, .width = 6, .height = 9},  // $
    {.x = 30, .y = 0, .width = 6, .height = 9},  // %
    {.x = 36, .y = 0, .width = 6, .height = 9},  // &
    {.x = 42, .y = 0, .width = 2, .height = 9},  // '
    {.x = 48, .y = 0, .width = 3, .height = 9},  // (
    {.x = 54, .y = 0, .width = 3, .height = 9},  // )
    {.x = 60, .y = 0, .width = 4, .height = 9},  // *
    {.x = 66, .y = 0, .width = 6, .height = 9},  // +
    {.x = 72, .y = 0, .width = 3, .height = 9},  // ,
    {.x = 78, .y = 0, .width = 6, .height = 9},  // -
    {.x = 84, .y = 0, .width = 2, .height = 9},  // .
    {.x = 90, .y = 0, .width = 6, .height = 9},  // /
    {.x = 0, .y = 9, .width = 6, .height = 9},   // 0
    {.x = 6, .y = 9, .width = 6, .height = 9},   // 1
    {.x = 12, .y = 9, .width = 6, .height = 9},  // 2
    {.x = 18, .y = 9, .width = 6, .height = 9},  // 3
    {.x = 24, .y = 9, .width = 6, .height = 9},  // 4
    {.x = 30, .y = 9, .width = 6, .height = 9},  // 5
    {.x = 36, .y = 9, .width = 6, .height = 9},  // 6
    {.x = 42, .y = 9, .width = 6, .height = 9},  // 7
    {.x = 48, .y = 9, .width = 6, .height = 9},  // 8
    {.x = 54, .y = 9, .width = 6, .height = 9},  // 9
    {.x = 60, .y = 9, .width = 2, .height = 9},  // :
    {.x = 66, .y = 9, .width = 3, .height = 9},  // ;
    {.x = 72, .y = 9, .width = 6, .height = 9},  // <
    {.x = 78, .y = 9, .width = 6, .height = 9},  // =
    {.x = 84, .y = 9, .width = 6, .height = 9},  // >
    {.x = 90, .y = 9, .width = 6, .height = 9},  // ?
    {.x = 0, .y = 18, .width = 6, .height = 9},  // @
    {.x = 6, .y = 18, .width = 6, .height = 9},  // A
    {.x = 12, .y = 18, .width = 6, .height = 9}, // B
    {.x = 18, .y = 18, .width = 6, .height = 9}, // C
    {.x = 24, .y = 18, .width = 6, .height = 9}, // D
    {.x = 30, .y = 18, .width = 6, .height = 9}, // E
    {.x = 36, .y = 18, .width = 6, .height = 9}, // F
    {.x = 42, .y = 18, .width = 6, .height = 9}, // G
    {.x = 48, .y = 18, .width = 6, .height = 9}, // H
    {.x = 54, .y = 18, .width = 4, .height = 9}, // I
    {.x = 60, .y = 18, .width = 5, .height = 9}, // J
    {.x = 66, .y = 18, .width = 6, .height = 9}, // K
    {.x = 72, .y = 18, .width = 6, .height = 9}, // L
    {.x = 78, .y = 18, .width = 6, .height = 9}, // M
    {.x = 84, .y = 18, .width = 6, .height = 9}, // N
    {.x = 90, .y = 18, .width = 6, .height = 9}, // O
    {.x = 0, .y = 27, .width = 6, .height = 9},  // P
    {.x = 6, .y = 27, .width = 6, .height = 9},  // Q
    {.x = 12, .y = 27, .width = 6, .height = 9}, // R
    {.x = 18, .y = 27, .width = 6, .height = 9}, // S
    {.x = 24, .y = 27, .width = 6, .height = 9}, // T
    {.x = 30, .y = 27, .width = 6, .height = 9}, // U
    {.x = 36, .y = 27, .width = 6, .height = 9}, // V
    {.x = 42, .y = 27, .width = 6, .height = 9}, // W
    {.x = 48, .y = 27, .width = 6, .height = 9}, // X
    {.x = 54, .y = 27, .width = 6, .height = 9}, // Y
    {.x = 60, .y = 27, .width = 6, .height = 9}, // Z
    {.x = 66, .y = 27, .width = 3, .height = 9}, // [
    {.x = 72, .y = 27, .width = 6, .height = 9}, // backslash
    {.x = 78, .y = 27, .width = 3, .height = 9}, // ]
    {.x = 84, .y = 27, .width = 4, .height = 9}, // ^
    {.x = 90, .y = 27, .width = 6, .height = 9}, // _
    {.x = 0, .y = 36, .width = 3, .height = 9},  // `
    {.x = 6, .y = 36, .width = 6, .height = 9},  // a
    {.x = 12, .y = 36, .width = 6, .height = 9}, // b
    {.x = 18, .y = 36, .width = 6, .height = 9}, // c
    {.x = 24, .y = 36, .width = 6, .height = 9}, // d
    {.x = 30, .y = 36, .width = 6, .height = 9}, // e
    {.x = 36, .y = 36, .width = 5, .height = 9}, // f
    {.x = 42, .y = 36, .width = 6, .height = 9}, // g
    {.x = 48, .y = 36, .width = 5, .height = 9}, // h
    {.x = 54, .y = 36, .width = 2, .height = 9}, // i
    {.x = 60, .y = 36, .width = 4, .height = 9}, // j
    {.x = 66, .y = 36, .width = 5, .height = 9}, // k
    {.x = 72, .y = 36, .width = 2, .height = 9}, // l
    {.x = 78, .y = 36, .width = 6, .height = 9}, // m
    {.x = 84, .y = 36, .width = 5, .height = 9}, // n
    {.x = 90, .y = 36, .width = 6, .height = 9}, // o
    {.x = 0, .y = 45, .width = 6, .height = 9},  // p
    {.x = 6, .y = 45, .width = 6, .height = 9},  // q
    {.x = 12, .y = 45, .width = 6, .height = 9}, // r
    {.x = 18, .y = 45, .width = 6, .height = 9}, // s
    {.x = 24, .y = 45, .width = 5, .height = 9}, // t
    {.x = 30, .y = 45, .width = 5, .height = 9}, // u
    {.x = 36, .y = 45, .width = 6, .height = 9}, // v
    {.x = 42, .y = 45, .width = 6, .height = 9}, // w
    {.x = 48, .y = 45, .width = 6, .height = 9}, // x
    {.x = 54, .y = 45, .width = 6, .height = 9}, // y
    {.x = 60, .y = 45, .width = 5, .height = 9}, // z
    {.x = 66, .y = 45, .width = 4, .height = 9}, // {
    {.x = 72, .y = 45, .width = 2, .height = 9}, // |
    {.x = 78, .y = 45, .width = 4, .height = 9}, // }
    {.x = 84, .y = 45, .width = 6, .height = 9}, // ~
    {.x = 90, .y = 45, .width = 6, .height = 9}  // (invalid)
};

#define FONT_FIRST_CHAR '!'

/* -------------------------------------------------------------------------
 * Predefined halt screen colors (exported for platform halt extensions)
 * ------------------------------------------------------------------------- */

const HaltColor ColorRed   = {160, 0, 0};
const HaltColor ColorGreen = {0, 160, 0};
const HaltColor ColorBlue  = {0, 0, 128};

/* -------------------------------------------------------------------------
 * Low-level GPU helpers
 * ------------------------------------------------------------------------- */

static void _waitForGP0Ready(void)
{
    while (!(GPU_GP1 & GP1_STAT_CMD_READY))
        __asm__ volatile("");
}

static void _waitForDMADone(void)
{
    while (DMA_CHCR(DMA_GPU) & DMA_CHCR_ENABLE)
        __asm__ volatile("");
}

/* Send a block of data to VRAM via DMA slice mode. Data must be 4-byte aligned. */
static void _sendVRAMData(
    const void *data,
    int x, int y,
    int width, int height)
{
    _waitForDMADone();

    /* DMA operates in 32-bit words; width/height here are in 16bpp pixels,
     * so total 16bpp pixels / 2 = number of 32-bit words. */
    size_t length    = ((size_t)width * (size_t)height + 1) / 2;
    size_t chunkSize = (length < DMA_MAX_CHUNK_SIZE) ? length : DMA_MAX_CHUNK_SIZE;
    size_t numChunks = length / chunkSize;

    _waitForGP0Ready();
    GPU_GP0 = gp0_vramWrite();
    GPU_GP0 = gp0_xy(x, y);
    GPU_GP0 = gp0_xy(width, height);

    DMA_MADR(DMA_GPU) = (uint32_t)data;
    DMA_BCR(DMA_GPU)  = chunkSize | (numChunks << 16);
    DMA_CHCR(DMA_GPU) = 0 | DMA_CHCR_WRITE | DMA_CHCR_MODE_SLICE | DMA_CHCR_ENABLE;
}

/* -------------------------------------------------------------------------
 * Public GPU primitives
 * ------------------------------------------------------------------------- */

void psx_gpu_rectangle(HaltColor color, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    _waitForGP0Ready();
    GPU_GP0 = gp0_rgb(color.r, color.g, color.b) | gp0_vramFill();
    GPU_GP0 = gp0_xy(x, y);
    GPU_GP0 = gp0_xy(w, h);
}

void psx_gpu_fillScreen(HaltColor color, uint16_t x, uint16_t y)
{
    psx_gpu_rectangle(color, x, y, SCREEN_W, SCREEN_H);
}

int psx_gpu_drawText(
    HaltScreenFont *font,
    const char *str,
    int x, int y,
    int w, int h)
{
    if (!font || !str) return -1;

    _waitForGP0Ready();
    GPU_GP0 = gp0_texpage(font->page, false, false);

    int startX = x;
    int startY = y;
    int curX   = x;
    int curY   = y;

    for (; *str; str++)
    {
        char ch = *str;

        /* Wrap if we've exceeded the right bound */
        if (curX >= (startX + w))
        {
            curX = startX;
            curY += font->fontSize;
        }

        /* Clip vertically */
        if ((curY - startY) + font->fontSize > h)
            break;

        switch (ch)
        {
        case '\t':
            /* Advance to next tab stop */
            curX += font->tabWidth - 1;
            curX -= curX % font->tabWidth;
            continue;

        case '\r':
            curX = startX;
            continue;

        case '\n':
            curX = startX;
            curY += font->fontSize;
            continue;

        case ' ':
            curX += font->spaceWidth;
            continue;

        default:
            /* Map non-ASCII or below-table characters to the invalid glyph */
            if ((unsigned char)ch < FONT_FIRST_CHAR || (unsigned char)ch > 0x7f)
                ch = 0x7f;
            break;
        }

        const HaltGlyph *g = &fontGlyphs[(unsigned char)ch - FONT_FIRST_CHAR];

        _waitForGP0Ready();
        GPU_GP0 = gp0_rectangle(true, true, true);
        GPU_GP0 = gp0_xy(curX, curY);
        GPU_GP0 = gp0_uv(font->u + g->x, font->v + g->y, font->clut);
        GPU_GP0 = gp0_xy(g->width, g->height);

        curX += g->width;
    }

    return 0;
}

/* -------------------------------------------------------------------------
 * GPU initialisation and font upload
 *
 * Note: _sendVRAMData uses DMA_MAX_CHUNK_SIZE defined at the top of the file.
 * ------------------------------------------------------------------------- */

static void _setupGPU(void)
{
    int x    = 0x760;
    int mode = GPU_GP1 & GP1_STAT_FB_MODE_BITMASK;
    int y    = (mode == GP1_STAT_FB_MODE_PAL) ? 0xa3 : 0x88;

    GP1HorizontalRes hres = GP1_HRES_320;
    GP1VerticalRes vres   = GP1_VRES_256;

    int offsetX = (SCREEN_W * gp1_clockMultiplierH(hres)) / 2;
    int offsetY = (SCREEN_H / gp1_clockDividerV(vres)) / 2;

    GPU_GP1 = gp1_resetGPU();
    GPU_GP1 = gp1_fbRangeH(x - offsetX, x + offsetX);
    GPU_GP1 = gp1_fbRangeV(y - offsetY, y + offsetY);
    GPU_GP1 = gp1_fbMode(hres, vres,
                         (mode == GP1_STAT_FB_MODE_PAL)
                             ? GP1_MODE_PAL
                             : GP1_MODE_NTSC,
                         false, GP1_COLOR_16BPP);

    /* Single framebuffer at (0,0), no double buffering */
    GPU_GP0 = gp0_texpage(0, false, false);
    GPU_GP0 = gp0_fbOffset1(0, 0);
    GPU_GP0 = gp0_fbOffset2(SCREEN_W - 1, SCREEN_H - 1);
    GPU_GP0 = gp0_fbOrigin(0, 0);

    GPU_GP1 = gp1_fbOffset(0, 0);
    GPU_GP1 = gp1_dispBlank(false);

    /* Enable GPU DMA channel */
    DMA_DPCR |= DMA_DPCR_CH_ENABLE(DMA_GPU);
    GPU_GP1 = gp1_dmaRequestMode(GP1_DREQ_GP0_WRITE);
}

static HaltScreenFont _uploadFont(void)
{
    /* Upload the 4bpp image data. The image is 96 pixels wide; in 4bpp each
     * VRAM "pixel" holds two source pixels, so the effective VRAM width is
     * FONT_IMAGE_W / 4. */
    _sendVRAMData(
        _halt_font_image,
        FONT_VRAM_X, FONT_VRAM_Y,
        FONT_IMAGE_W / 4, FONT_IMAGE_H);
    _waitForDMADone();

    /* Upload the 16-colour palette (16 x 16bpp entries = 16 pixels wide) */
    _sendVRAMData(
        _halt_font_palette,
        FONT_PALETTE_X, FONT_PALETTE_Y,
        FONT_NUM_COLORS, 1);
    _waitForDMADone();

    GPU_GP0 = gp0_flushCache();

    HaltScreenFont font;
    font.page = gp0_page(
        FONT_VRAM_X / 64,
        FONT_VRAM_Y / 256,
        GP0_BLEND_SEMITRANS,
        FONT_COLOR_DEPTH);
    font.clut = gp0_clut(FONT_PALETTE_X / 16, FONT_PALETTE_Y);
    /* U/V base: where the texture sits within its 64px-wide texpage column */
    font.u          = (uint8_t)(FONT_VRAM_X % 64);
    font.v          = (uint8_t)(FONT_VRAM_Y % 256);
    font.fontSize   = 10; /* FONT_LINE_HEIGHT from the ps1-bare-metal example */
    font.spaceWidth = 4;
    font.tabWidth   = 32;
    return font;
}

/* -------------------------------------------------------------------------
 * Post-halt callback
 * ------------------------------------------------------------------------- */

static PostHaltFunc _phFunc = NULL;

void psx_halt_append_func(PostHaltFunc func)
{
    if (func) _phFunc = func;
}

/* -------------------------------------------------------------------------
 * Public entry point
 * ------------------------------------------------------------------------- */

void psx_halt_screen_show(const char *string)
{
    /* Mask all IRQs at the controller level - we don't trust any handlers */
    IRQ_MASK = 0;

    /* Reset the GPU to a known state and upload our built-in font */
    _setupGPU();
    HaltScreenFont font = _uploadFont();

    /* Draw a solid blue background */
    psx_gpu_fillScreen(ColorBlue, 0, 0);

    /* Print the error message, leaving a small margin */
    psx_gpu_drawText(&font, string, 5, 5, SCREEN_W - 10, SCREEN_H - 10);

    /* Hand off to any platform-specific post-halt handler */
    if (_phFunc) _phFunc(&font);

    /* Spin forever */
    for (;;)
        __asm__ volatile("");
}
