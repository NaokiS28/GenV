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
    typedef struct HaltScreenFont
    {
        uint16_t page;  // GP0 texpage word for the font spritesheet
        uint16_t clut;  // GP0 CLUT attribute word for the font palette
        uint8_t u;      // U base offset within the texpage (pixels)
        uint8_t v;      // V base offset within the texpage (pixels)
        int fontSize;   // Line height in pixels
        int spaceWidth; // Width of a space character in pixels
        int tabWidth;   // Width of a tab stop in pixels
    } HaltScreenFont;

    typedef struct HaltColor
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t attr;   // Not used

    } HaltColor;

    typedef struct HSConsole
    {
        int  (*const init_driver)(int baud);    // Called by Halt Screen when starting, Data is always 8N1
        void (*driver_update_cb)();             // Set by Halt Screen, driver should call this at minimum every 10ms
        void (*const puts)(const char *str);    // Print string function for Halt Screen
        void (*const putchar)(const char c);    // Print char function for Halt Screen
        void (*const end_driver)();             // Called by Halt Screen when closing
    } HSConsole;

    typedef struct HSDriver
    {
        char id[8];                     // 7 Character ID - Not currently used
        int (*const init_driver)();     // Called by Halt Screen when starting
        void (*const update_driver)();  // Called by Halt Screen (or console) during update pass
        void (*const end_driver)();     // Called by Halt Screen when closing
    } HSDriver;

    typedef enum HSDriverError : int
    {
        HS_DRIVER_OK        = 0,
        HS_DRIVER_INVALID   = -1,
        HS_DRIVER_LIST_FULL = -2,
    } HSDriverError;

    typedef struct HSExtension
    {
        void (*const init_halt)(HSDriver **list);   // Called by Halt Screen when starting
        void (*const show_halt)(HaltScreenFont *font, uint16_t x, uint16_t y, uint16_t w, uint16_t h);  // Called by Halt Screen when drawing
        void (*const exit_halt)(HaltScreenFont *font, uint16_t x, uint16_t y, uint16_t w, uint16_t h);  // Called by Halt Screen when closing
        void (*cb_exit)(); // This is provided by the main haltscreen to allow the extension to exit.
    } HSExtension;

    typedef enum HSExtensionError : int
    {
        HS_EXT_OK       = 0,
        HS_EXT_INVALID  = -1,
        HS_EXT_ASSIGNED = -2,
    } HSExtensionError;

    // Registers a hardware device driver which is essential for the
    // halt screen to use. This can be, for example, a watchdog timer.
    int genv_halt_register_driver(HSDriver *driver);

    // Registers functions to be called during init, update and exit.
    // Used by platform-specific code (e.g. extra hardware or timeouts
    // before letting watchdog expire). Only one may be registered.
    int genv_halt_register_extension(HSExtension *ext);

    // Resets the GPU and displays the given error string on a blue screen.
    void genv_halt_screen_show(const char *string);

    // Tells GenV's haltscreen which driver to use for console logging.
    int genv_halt_set_console(HSConsole *console);

    // Tells GenV's haltscreen what baud rate to use. Format is always 8N1
    void genv_halt_set_console_baud(int baud);

    #define GENV_HALT_TEXT_DETAIL_LEN 1024
    #define GENV_HALT_TEXT_TITLE_LEN 64
    #define GENV_HALT_TEXT_SHORT_LEN 128
    extern char genv_halt_text_detail[]; // Detailed info
    extern char genv_halt_text_title[]; // Short reason string - Divide by 0, etc.
    extern char genv_halt_text_short[]; // Short info

#ifdef __cplusplus
}
#endif

#endif