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

#ifndef GENV_PSX_HALT_C
#define GENV_PSX_HALT_C

#include "halt.h"
#include "psx/psx/registers.hpp"
#include "psx/psx/video/gpucmd.h"
#include <string.h>

typedef struct
{
    uint16_t r;
    uint16_t g;
    uint16_t b;
} Color;

Color ColorRed = {160, 0, 0};
Color ColorGreen = {0, 160, 0};
Color ColorBlue = {0, 0, 160};

void psx_gpu_waitForGP0Ready(void)
{
    // Block until the GPU reports to be ready to accept commands through its
    // status register (which has the same address as GP1 but is read-only).
    while (!(GPU_GP1 & GP1_STAT_CMD_READY))
        __asm__ volatile("");
}

void psx_gpu_setResolution(int mode, int width, int height)
{
    // Set the origin of the displayed framebuffer. These "magic" values,
    // derived from the GPU's internal clocks, will center the picture on most
    // displays and upscalers.
    int x = 0x760;
    int y = (mode == GP1_MODE_PAL) ? 0xa3 : 0x88;

    // Set the resolution. The GPU provides a number of fixed horizontal (256,
    // 320, 368, 512, 640) and vertical (240-256, 480-512) resolutions to pick
    // from, which affect how fast pixels are output and thus how "stretched"
    // the framebuffer will appear.
    GP1HorizontalRes horizontalRes = GP1_HRES_320;
    GP1VerticalRes verticalRes = GP1_VRES_256;

    // Set the number of displayed rows and columns. These values are in GPU
    // clock units rather than pixels, thus they are dependent on the selected
    // resolution.
    int offsetX = (width * gp1_clockMultiplierH(horizontalRes)) / 2;
    int offsetY = (height / gp1_clockDividerV(verticalRes)) / 2;

    // Hand all parameters over to the GPU by sending GP1 commands.
    GPU_GP1 = gp1_resetGPU();
    GPU_GP1 = gp1_fbRangeH(x - offsetX, x + offsetX);
    GPU_GP1 = gp1_fbRangeV(y - offsetY, y + offsetY);
    GPU_GP1 = gp1_fbMode(
        horizontalRes,
        verticalRes,
        mode,
        false,
        GP1_COLOR_16BPP);
}

void psx_gpu_fillScreen(Color color, uint16_t x, uint16_t y)
{
    psx_gpu_waitForGP0Ready();
    GPU_GP0 = (gp0_rgb(color.r, color.g, color.b) | gp0_vramFill());
    GPU_GP0 = (gp0_xy(x, y));
    GPU_GP0 = (gp0_xy(320, 240));
}

void psx_gpu_init()
{
    // Barebones direct reset of the GPU
    int gpuMode = (GPU_GP1 & GP1_STAT_FB_MODE_BITMASK);
    GPU_GP1 = gp1_resetGPU();
    GPU_GP1 = gp1_resetFIFO();

    psx_gpu_setResolution(gpuMode, 320, 240);

    GPU_GP0 = (gp0_texpage(0, true, false));
    GPU_GP0 = (gp0_fbOffset1(0, 0));
    GPU_GP0 = (gp0_fbOffset2(320 - 1, 240 - 1));
    GPU_GP0 = (gp0_fbOrigin(0, 0));

    GPU_GP1 = gp1_fbOffset(0, 0);
    GPU_GP1 = gp1_dispBlank(false);
}

int psx_gpu_drawTextureObject(
    const HaltScreenFont *ptObj,
    int x, int y, int w, int h,
    uint16_t u, uint16_t v)
{
    if (!ptObj) return -1;

    psx_gpu_waitForGP0Ready();
    GPU_GP0 = (gp0_rgb(255, 255, 255) | gp0_rectangle(true, true, false));
    GPU_GP0 = (gp0_xy(x, y));
    GPU_GP0 = (gp0_uv(ptObj->tpage.xoffset + u, ptObj->tpage.yoffset + v, gp0_clut((ptObj->clut.x / 16), ptObj->clut.y)));
    GPU_GP0 = (gp0_xy(w, h));
    return 0;
}

int psx_gpu_drawText(HaltScreenFont *ptObj, const char *str, int x, int y, int w, int h)
{
    if (!ptObj) return -1;

    psx_gpu_waitForGP0Ready();
    GPU_GP0 = (gp0_texpage(gp0_page(ptObj->tpage.x, ptObj->tpage.y, GP0_BLEND_ADD, GP0_COLOR_4BPP), true, false));

    // Print in selected color
    int startX = x;
    int i = 0;
    bool drawText = true;

    while (drawText)
    {
        if (x >= (w - startX))
        {
            x = startX;
            y += ptObj->fontSize;
        }
        if (y + ptObj->fontSize >= h)
        {
            drawText = false;
            break;
        }

        switch (str[i])
        {
        case '\0':
            drawText = false;
            break;
        case '\t':
            x += ptObj->tabWidth;
            break;
        case '\r':
            x = startX;
            break;
        case '\n':
            y += ptObj->fontSize;
            break;
        case ' ':
            x += ptObj->spaceWidth;
            break;
        default:
        {
            const FontGlyph c = glyph_get(ptObj->table, ptObj->numBuckets, str[i]);
            // TODO: Somewhere, somehow, font height is not being stored in the font file correctly
            psx_gpu_drawTextureObject(ptObj, x, y, c.w, ptObj->fontSize, c.x, c.y);
            x += c.w;
            break;
        }
        }
        i++;
    }
    return 0;
}

HaltScreenFont hsFont;
void psx_gpu_register_font(HaltScreenFont *font)
{
    memcpy(&hsFont, font, sizeof(HaltScreenFont));
}

void psx_gpu_halt_screen(const char *string)
{
    // System has crashed/halted and is beyond recovery, reset GPU to known state
    psx_gpu_init();

    // Draw blue background. We are not double buffering any more, so draw only once.
    psx_gpu_fillScreen(ColorBlue, 0, 0);

    // Print the error screen with the default font file
    psx_gpu_drawText(&hsFont, string, 5, 5, 315, 235);
}

#endif