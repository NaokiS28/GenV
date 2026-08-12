/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * cartridge.hpp - Created on 23-02-2026
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

#include "psx/common/system/registers.h"
#include "psx/sys573/io/asic.hpp"

namespace System573::IO::Cartridge
{
    enum class GPIODir : bool
    {
        Input  = true,
        Output = false
    };

    int SetOutPort(uint8_t data);
    int GetInPort(uint8_t *data);

    inline bool InPortAvailable()
    {
        return (ASIC::Regs::MiscIn & ASIC::IN_CART_IRDY) != ASIC::IN_NONE;
    }

    inline bool OutPortFull()
    {
        return (ASIC::Regs::MiscIn & ASIC::IN_CART_DRDY) != ASIC::IN_NONE;
    }

    // ----------
    // Cart GPIO
    // ----------
    inline bool GetGPIOState()
    {
        return (ASIC::Regs::MiscIn & ASIC::IN_CART_SDA) != ASIC::IN_NONE;
    }

    // Writes to D0 of the output port. Note this will trigger the DRDY flag
    int SetGPIOState(bool state);

    // Inside of flash.cpp GPIO direction function as need to track bank select
    extern void SetGPIODirection(GPIODir state);
} // namespace System573::IO::Cartridge