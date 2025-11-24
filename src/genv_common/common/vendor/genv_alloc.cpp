/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * genv_alloc.cpp - Created on 20-11-2025
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

#include <stdlib.h>
#include <string.h>
#include "genv_alloc.hpp"

// Replace this with your platform allocator later.
static void *aligned_alloc_fallback(uint32_t alignment, uint32_t size)
{
    // Overalign manually: allocate (size + alignment) and adjust.
    uintptr_t raw     = (uintptr_t)malloc(size + alignment);
    uintptr_t aligned = (raw + (alignment - 1)) & ~(alignment - 1);
    return (void *)aligned;
}

class HeapAllocator : public IGenAllocator
{
public:
    AllocInfo allocate(uint32_t size, uint32_t alignment) override
    {
        void *p = aligned_alloc_fallback(alignment, size);
        if (!p) return {nullptr, 0, alignment};

        memset(p, 0, size);
        return {p, size, alignment};
    }

    void deallocate(void *ptr) override
    {
        free(ptr);
    }
};
