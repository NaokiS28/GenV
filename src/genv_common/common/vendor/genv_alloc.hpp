/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * genv_gfx_alloc.hpp - Created on 20-11-2025
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

#include <stdint.h>
#include <stddef.h>
#include "common/services/services.hpp"

struct AllocInfo
{
    void *ptr;
    uint32_t size;
    uint32_t alignment;
};

class IGenAllocator
{
public:
    virtual AllocInfo allocate(uint32_t size, uint32_t alignment) = 0;
    virtual void deallocate(void *ptr)                            = 0;
};