/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * font_glyph.c - Created on 01-12-2025
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

#ifndef GENV_FONT_GLYPH_C
#define GENV_FONT_GLYPH_C

#include <stdint.h>
#include <stddef.h>

#include "font_glyph.h"

const FontGlyph *glyph_getHashTableEntry(
    const FontGlyph *table,
    size_t numBuckets,
    uint32_t id)
{
    uint32_t index = id & (numBuckets - 1);
    do
    {
        const FontGlyph *entry = &table[index];
        index = glyph_getChained(entry);

        if (glyph_getHash(entry) == id)
            return entry;
    } while (index);

    return NULL;
}

const FontGlyph glyph_get(const FontGlyph *table, size_t numBuckets, uint32_t id)
{
    const FontGlyph *entry = glyph_getHashTableEntry(
        table,
        numBuckets,
        id);

    if (entry != NULL)
        return *entry;
    else
        return glyph_get(table, numBuckets, FONT_INVALID_CHAR);
}

#endif