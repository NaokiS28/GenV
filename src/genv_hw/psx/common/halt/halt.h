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

// Self-contained halt screen library for PSX. Resets the GPU and renders an
// error message using a built-in font spritesheet. Assumes all prior execution
// context is invalid - no dependencies on the rest of GenV.

#pragma once
#ifndef GENV_PSX_HALT_H
#define GENV_PSX_HALT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // Metrics for the active halt screen font. The glyph table is internal to
    // halt.c; this struct exposes only what platform-specific post-halt
    // callbacks (e.g. System 573) need in order to draw additional text.
    typedef struct
    {
        uint16_t page;  // GP0 texpage word for the font spritesheet
        uint16_t clut;  // GP0 CLUT attribute word for the font palette
        uint8_t u;      // U base offset within the texpage (pixels)
        uint8_t v;      // V base offset within the texpage (pixels)
        int fontSize;   // Line height in pixels
        int spaceWidth; // Width of a space character in pixels
        int tabWidth;   // Width of a tab stop in pixels
    } HaltScreenFont;

    typedef struct
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } HaltColor;

    typedef void (*PostHaltFunc)(HaltScreenFont *font);

    // Registers a function to be called after the halt screen has been
    // displayed. Used by platform-specific code (e.g. System 573 watchdog
    // kick / countdown before reboot).
    void psx_halt_append_func(PostHaltFunc func);

    // Resets the GPU and displays the given error string on a blue screen.
    void psx_halt_screen_show(const char *string);

    // Draw a string using the halt screen font. x/y are the top-left origin;
    // w/h bound the text area. Returns 0 on success.
    int psx_gpu_drawText(
        HaltScreenFont *font,
        const char *str,
        int x, int y,
        int w, int h);

    // Fill a solid-color rectangle directly via GP0 VRAM fill.
    void psx_gpu_rectangle(HaltColor color, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

    // Fill the full 320x240 screen with a solid color.
    void psx_gpu_fillScreen(HaltColor color, uint16_t x, uint16_t y);

#ifdef __cplusplus
}
#endif

#endif