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
    inline uint8_t DIP()
    {
        return (ASIC::Regs::DipCart & 0x0F);
    }

    inline void TickWatchdog()
    {
        ASIC::Regs::Watchdog = 0;
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

    // ----------
    // Cart FIFO
    // ----------
    namespace SecurityCart
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

    } // namespace SecurityCart

    namespace Flash
    {
        enum FlashBank : uint8_t
        {
            FLASH   = 0,
            PCMCIA1 = 16,
            PCMCIA2 = 32
        };

        extern void SetBank(uint8_t bank);
    } // namespace Flash

} // namespace System573::IO