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

#include "jamma/jamma.hpp"      // IWYU pragma: export
#include "expansion/expbus.hpp" // IWYU pragma: export

#include "psx/common/system/registers.h"

namespace SystemGV::IO
{
#define GV_IO_BASE 0x1f000000
    namespace Regs
    {
        static volatile uint16_t &SCSI      = *_ADDR16(GV_IO_BASE | 0x000000);
        static volatile uint16_t &JammaP1   = *_ADDR16(GV_IO_BASE | 0x100000);
        static volatile uint16_t &JammaP2   = *_ADDR16(GV_IO_BASE | 0x100004);
        static volatile uint16_t &JammaP3P4 = *_ADDR16(GV_IO_BASE | 0x100008);
        static volatile uint16_t &Outputs   = *_ADDR16(GV_IO_BASE | 0x180000);
        static volatile uint16_t &ExpPort   = *_ADDR16(GV_IO_BASE | 0x680000);
        static volatile uint16_t &Watchdog  = *_ADDR16(GV_IO_BASE | 0x780000);
        static volatile uint16_t &EEPROMIn  = JammaP1;
        static volatile uint16_t &EEPROMOut = Outputs;
    } // namespace Regs

    enum class Outputs : uint8_t
    {
        NONE           = 0,
        EEPROM_MOSI    = (1 << 0),
        EEPROM_SS      = (1 << 1),
        EEPROM_SCLK    = (1 << 2),
        COIN_COUNTER_1 = (1 << 3),
        COIN_COUNTER_2 = (1 << 4),
        UNKNOWN_1      = (1 << 5),
        UNKNOWN_2      = (1 << 6),
        UNKNOWN_3      = (1 << 7),
    };

    ENABLE_BITWISE_OPS(Outputs);

    inline void TickWatchdog()
    {
        Regs::Watchdog = 0;
    }

    void SetOutputs(Outputs state);                       // Writes all outputs to the raw given data
    void SetOutputsMasked(Outputs outputs, Outputs mask); // Writes multiple bits using a mask to only change select bits
    void SetOutputBit(Outputs output, bool state);        // Sets/Unsets a single bit of the output data
    void PulseOutput(Outputs output);                     // Pulses output (only supports coin counters and JVS reset, blocking)
    void MuteAudio();                                     // Mutes the passed audio channels (only supports audio channels)
    void UnmuteAudio();                                   // Unmutes the passed audio channels (only supports audio channels)

} // namespace SystemGV::IO