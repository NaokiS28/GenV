/*
 * Originally from 573in1 - Copyright (C) 2022-2024 spicyjpeg
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

#include <stddef.h>
#include <stdint.h>

namespace util
{
    inline uint8_t dec2bcd(uint8_t n)
    {
        return (n / 10 * 16) + (n % 10);
    }
    inline uint8_t bcd2dec(uint8_t n)
    {
        return (n / 16 * 10) + (n % 16);
    }

    template <typename T>
    void ClearMemory(T *dest, size_t len)
    {
        memset(dest, 0, len);
    }

    // Sum a simple 16-bit checksum.
    uint16_t checksum(const void *data, size_t length);
} // namespace util