/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * font_glyph.h - Created on 01-12-2025
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
#ifndef GENV_FONT_GLYPH_H
#define GENV_FONT_GLYPH_H

#include <stdint.h>
#include <stddef.h>

#define METRICS_CODE_POINT_BITS 21
#define FONT_INVALID_CHAR 0xfffd

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        uint32_t c; // Unicode codepoint
        uint8_t x, y;
        uint8_t w, h;
    } FontGlyph;

    inline uint32_t glyph_getHash(const FontGlyph *g)
    {
        return g->c & ((1 << METRICS_CODE_POINT_BITS) - 1);
    }

    inline uint32_t glyph_getChained(const FontGlyph *g)
    {
        return g->c >> METRICS_CODE_POINT_BITS;
    }

    const FontGlyph *glyph_getHashTableEntry(const FontGlyph *table, size_t numBuckets, uint32_t id);
    const FontGlyph glyph_get(const FontGlyph *table, size_t numBuckets, uint32_t id);

#ifdef __cplusplus
}
#endif

#endif