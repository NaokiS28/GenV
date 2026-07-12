/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * halt_ext.h - Created on 28-06-2026
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

#pragma once

#include "halt.h"

#ifdef __cplusplus
extern "C"
{
#endif

    // Draw a string using the halt screen font. x/y are the top-left origin;
    // w/h bound the text area. Returns 0 on success.
    int genv_gpu_drawText(
        HaltScreenFont *font,
        const char *str,
        int x, int y,
        int w, int h);

    // Fill a solid-color rectangle directly via GP0 VRAM fill.
    void genv_gpu_rectangle(HaltColor color, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

    // Fill the full 320x240 screen with a solid color.
    void genv_gpu_fillScreen(HaltColor color, uint16_t x, uint16_t y);

#ifdef __cplusplus
}
#endif