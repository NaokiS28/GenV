/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * io.c - Created on 26-11-2025
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

#include <stdint.h>
#include <stdbool.h>

#include "io.hpp"

namespace System573::IO
{
    namespace EXTOUT
    {
        uint8_t ext_out_state = 0;

        void set_state(uint8_t data)
        {
            ext_out_state      = data;
            ASIC::Regs::ExtOut = ext_out_state;
        }

        uint8_t get_state()
        {
            return ext_out_state;
        }

        void write_bit(uint8_t bit, bool state)
        {
            if (bit > 7) return;
            if (state)
                ext_out_state |= (1 << bit);
            else
                ext_out_state &= ~(1 << bit);
            ASIC::Regs::ExtOut = ext_out_state;
        }
    } // namespace EXTOUT

    namespace SecurityCart
    {
        constexpr uint8_t OutPortGPIOMask = ~(1 << 0);
        uint8_t OutPortState              = 0;

        int SetGPIOState(bool state)
        {
            auto x = ASIC::Regs::MiscIn & ASIC::IN_CART_DRDY;
            if (x != ASIC::IN_NONE)
            {
                OutPortState        = (OutPortState & OutPortGPIOMask) | state;
                ASIC::Regs::CartOut = OutPortState;
                return 0;
            }
            return 1;
        }

        int SetOutPort(uint8_t data)
        {
            auto x = ASIC::Regs::MiscIn & ASIC::IN_CART_DRDY;
            if (x != ASIC::IN_NONE)
            {
                OutPortState        = data;
                ASIC::Regs::CartOut = OutPortState;
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
    } // namespace SecurityCart
} // namespace System573::IO