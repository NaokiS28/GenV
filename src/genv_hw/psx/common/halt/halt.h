/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * halt.h - Created on 01-12-2025
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

// Very basic GPU driver designed to reset the GPU and use bare C to draw out
// an error screen.
// TODO: Currently relies on a existing font from GenV, but trying to assume
// the worst case scenario, we shouldnt trust a font to be uploaded.

#pragma once
#ifndef GENV_PSX_HALT_H
#define GENV_PSX_HALT_H

#include <stdint.h>
#include <stddef.h>
#include "common/objects/fonts/font_glyph.h"

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct
    {
        struct
        {
            int x;
            int y;
            int xoffset;
            int yoffset;
        } tpage;
        struct
        {
            int x;
            int y;
        } vram;
        struct
        {
            int x;
            int y;
        } clut;
        int tabWidth;
        int spaceWidth;
        int fontSize;
        const FontGlyph *table;
        size_t numBuckets;
    } HaltScreenFont;

    typedef struct
    {
        uint16_t r;
        uint16_t g;
        uint16_t b;
    } HaltColor;

    typedef void (*PostHaltFunc)(HaltScreenFont *font);

    // Allows passing of a function that will be ran after the halt screen has been displayed.
    void psx_halt_append_func(PostHaltFunc func);

    // Reinits the GPU and shows the given string on screen.
    void psx_halt_screen_show(const char *string);

    int psx_gpu_drawText(HaltScreenFont *ptObj, const char *str, int x, int y, int w, int h);
    void psx_gpu_rectangle(HaltColor color, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void psx_gpu_fillScreen(HaltColor color, uint16_t x, uint16_t y);

#ifdef __cplusplus
}
#endif

#endif