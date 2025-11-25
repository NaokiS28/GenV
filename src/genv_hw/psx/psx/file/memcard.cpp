/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * memcard.cpp - Created on 09-11-2025
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

#include "memcard.hpp"
#include <printf.h>

uint8_t PSX_MemoryCard_xor(void *data, size_t len)
{
    uint8_t sum = 0;
    for (size_t i = 0; i < len; i++)
    {
        sum ^= static_cast<uint8_t *>(data)[i];
    }
    return sum;
}

void PSX_MemoryCard_format_filename(
    char **out,
    uint32_t titleID,
    const char *filename,
    MemCardRegion region,
    MemCardLicense license,
    bool pocketexe)
{
    if (!*out)
        return;

    memcpy(out[0], (const void *)region, 2);
    memcpy(out[2], (const void *)license, 4);
    *out[7] = (pocketexe ? 'P' : '-');
    snprintf(out[8], 6, "%05d");
    memcpy(out[8], filename, 8);
}