/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * new_a.cc - Created on 09-07-2025
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

#include <cstdlib>
#include <new>

#include <bmcxx_config.h>

// new: array

void *operator new[](size_t sz)
{
    void *r = malloc(sz);
#if !BMCXX_DISABLE_EXCEPTIONS
    if (r == nullptr)
    {
        throw std::bad_alloc();
    }
    // (see notes in new_r/new_n)
#endif
    return r;
}
