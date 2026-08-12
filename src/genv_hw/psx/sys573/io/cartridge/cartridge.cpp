/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * cartridge.cpp - Created on 19-07-2026
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

#include "cartridge.hpp"
#include "psx/sys573/io/asic.hpp"

namespace System573::IO::Cartridge
{
    static volatile uint16_t &CartOut = *_ADDR16(DEV0_BASE | 0x6a0000);
    static volatile uint16_t &CartIn  = *_ADDR16(DEV0_BASE | 0x6a0000);

    constexpr uint8_t OutPortGPIOMask = ~(1 << 0);
    uint8_t OutPortState              = 0;

    int SetGPIOState(bool state)
    {
        auto x = ASIC::Regs::MiscIn & ASIC::IN_CART_DRDY;
        if (x != ASIC::IN_NONE)
        {
            OutPortState = (OutPortState & OutPortGPIOMask) | state;
            CartOut      = OutPortState;
            return 0;
        }
        return 1;
    }

    int SetOutPort(uint8_t data)
    {
        auto x = ASIC::Regs::MiscIn & ASIC::IN_CART_DRDY;
        if (x != ASIC::IN_NONE)
        {
            OutPortState = data;
            CartOut      = OutPortState;
            return 0;
        }
        return 1;
    }

    int GetInPort(uint8_t *data)
    {
        auto x = ASIC::Regs::MiscIn & ASIC::IN_CART_IRDY;
        if (x != ASIC::IN_NONE)
        {
            *data = (ASIC::Regs::DipCart & 0xFF00) >> 8;
            return 0;
        }
        return 1;
    }

} // namespace System573::IO::Cartridge