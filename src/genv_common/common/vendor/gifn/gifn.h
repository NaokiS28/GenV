/*
 * GIFN - Copyright (C) 2025 NaokiS, Echo
 * ====
 * Get Image File Now - minimal GIF extractor
 * Because GRIFN wouldn't sound like it wasn't a GIF library.
 *
 * Reads GIF89a/87a streams into raw indexed image frames.
 * Caller handles final format/rendering.
 *
 * GIFN is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * GIFN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * GIFN. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define GIFN_OK 0
#define GIFN_ERR_FORMAT -1
#define GIFN_ERR_TRUNCATED -2
#define GIFN_ERR_NOMEM -3
#define GIFN_ERR_BOUNDS -4

/*Compile the default allocators (C's free and malloc). If you disable this,
you can define the functions gifn_free, gifn_malloc and gifn_realloc in your
source files with custom allocators.*/
#ifndef GIFN_NO_COMPILE_ALLOCATORS
/*pass -DGIFN_NO_COMPILE_ALLOCATORS to the compiler to disable the built-in ones,
or comment out GIFN_COMPILE_ALLOCATORS below*/
#define GIFN_COMPILE_ALLOCATORS
#endif

    typedef struct
    {
        uint8_t r, g, b;
    } GIF_Color; // 24-bit color entry

    typedef struct
    {
        uint16_t x, y;
        uint16_t w, h;
        uint8_t flags;    /* Local table flag, interlace, etc */
        uint16_t lctSize; /* Local Color Table entry count (0–256) */
        GIF_Color *lct;   /* Local Color Table, if present */
        uint8_t *indices; /* Raw decoded pixel indices (w*h) */
    } GIF_ImageDescriptor;

    /* Graphic Control Extension */
    typedef struct
    {
        uint16_t delayTime;     /* Delay in hundredths of a second */
        uint8_t transparent;    /* Transparency flag */
        uint8_t transparentIdx; /* Transparent color index if flag set */
        uint8_t disposal;       /* Disposal method */
    } GIF_GCE;

    typedef struct
    {
        uint16_t width;     /* Logical Screen Width */
        uint16_t height;    /* Logical Screen Height */
        uint8_t flags;      /* GCT flag, color resolution, sort flag, GCT size */
        uint8_t bgColorIdx; /* Background Color Index */
        uint8_t aspect;     /* Pixel Aspect Ratio */
        uint16_t gctSize;   /* Global Color Table entry count (0–256) */
        GIF_Color *gct;     /* Global Color Table */
    } GIF_Header;

    typedef struct
    {
        GIF_Header header;           /* Standard GIF header */
        GIF_GCE *framesGCE;          /* Graphic Control Extension for each frame[frameCount] */
        GIF_ImageDescriptor *frames; /* Image Descriptor for each frame[frameCount] */
        size_t *frameLength;         /* Data length of each frame[frameCount] */
        uint16_t frameCount;

        void *_arena;      /* For internal use (do not touch) */
        size_t _arenaSize; /* For internal use (do not touch) */
    } GIF_Object;

    int gifn_load_memory(GIF_Object *gif, const uint8_t *in, size_t inSize);
    int gifn_load_file(GIF_Object *gif, const char *filepath);

    void gifn_cleanup(GIF_Object *gif);

    // Utility: convert a single color
    inline int gifn_color_as_xrgb(const GIF_Color ct)
    {
        return (0 | (ct.r << 16) | (ct.g << 8) | ct.b);
    }
    inline int gifn_color_as_xbgr(const GIF_Color ct)
    {
        return (0 | (ct.b << 16) | (ct.g << 8) | ct.r);
    }

    /**
     * Convert a color table to a 32-bit packed array (caller must delete[] result).
     */
    int gifn_color_table_as_u32(const GIF_Color *ct, const size_t size, uint32_t **out, const bool xbgr);

    /**
     * Convert a color table to a byte RGB array (caller must delete[] result).
     */
    int gifn_color_table_as_u8(const GIF_Color *ct, const size_t size, uint8_t **out, const bool xbgr);

    uint8_t gifn_get_bpp(const uint8_t flags);

#ifdef __cplusplus
}
#endif
