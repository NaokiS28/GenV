/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * flash.hpp - Created on 05-03-2026
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

#include "psx/common/system/registers.h"

namespace System573::IO::Flash
{

    enum FlashBank : uint8_t
    {
        FLASH   = 0,
        PCMCIA1 = 16,
        PCMCIA2 = 32
    };

    extern void SetBank(uint8_t bank);
} // namespace System573::IO::Flash