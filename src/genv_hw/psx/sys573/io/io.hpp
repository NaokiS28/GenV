/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * io.h - Created on 26-11-2025
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
#include <stdbool.h>

#include "common/util/enum_defs.hpp"
#include "psx/sys573/io/asic.hpp"

#include "jamma/jamma.hpp"      // IWYU pragma: export
#include "jvs/jvs.hpp"          // IWYU pragma: export
#include "jvs/jvs.hpp"          // IWYU pragma: export
#include "expansion/expbus.hpp" // IWYU pragma: export

namespace System573::IO
{
    static volatile uint16_t &Watchdog = *_ADDR16(DEV0_BASE | 0x5c0000);
    static volatile uint16_t &ExtOut   = *_ADDR16(DEV0_BASE | 0x600000);

    inline uint8_t DIP()
    {
        return (ASIC::Regs::DipCart & 0x0F);
    }

    inline void __attribute__((always_inline)) TickWatchdog()
    {
        Watchdog = 0;
    }

    // ----------
    // System 573 EXT Out
    // ----------
    namespace EXTOUT
    {
        void SetState(uint8_t data);
        uint8_t ReadState();
        void SetBit(uint8_t bit, bool state); // Sets/Unsets a single bit of the output data
    } // namespace EXTOUT

} // namespace System573::IO