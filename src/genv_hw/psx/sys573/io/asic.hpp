/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * asic.h - Created on 26-11-2025
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

#include "common/util/enum_defs.hpp"
#include "psx/common/registers.hpp" // IWYU pragma: keep
#include <stdint.h>
#include <stdbool.h>

namespace System573::IO::ASIC
{
    namespace Regs
    {
        static volatile uint16_t &MiscOut   = *_ADDR16(DEV0_BASE | 0x400000);
        static volatile uint16_t &DipCart   = *_ADDR16(DEV0_BASE | 0x400004);
        static volatile uint16_t &MiscIn    = *_ADDR16(DEV0_BASE | 0x400006);
        static volatile uint16_t &JammaMain = *_ADDR16(DEV0_BASE | 0x400008);
        static volatile uint16_t &JvsRxData = *_ADDR16(DEV0_BASE | 0x40000a);
        static volatile uint16_t &JammaExt1 = *_ADDR16(DEV0_BASE | 0x40000c);
        static volatile uint16_t &JammaExt2 = *_ADDR16(DEV0_BASE | 0x40000e);

        static volatile uint16_t &IdeReset  = *_ADDR16(DEV0_BASE | 0x560000);
        static volatile uint16_t &JvsTxData = *_ADDR16(DEV0_BASE | 0x680000);
    } // namespace Regs

    enum MiscOutput : uint16_t
    {
        OUT_NONE        = 0 << 0,
        OUT_ADC_DI      = 1 << 0,
        OUT_ADC_CS      = 1 << 1,
        OUT_ADC_CLK     = 1 << 2,
        OUT_COIN_COUNT1 = 1 << 3,
        OUT_COIN_COUNT2 = 1 << 4,
        OUT_AMP_ENABLE  = 1 << 5,
        OUT_CDDA_ENABLE = 1 << 6,
        OUT_SPU_ENABLE  = 1 << 7,
        OUT_JVS_RESET   = 1 << 8
    };

    ENABLE_BITWISE_OPS(MiscOutput);

    enum MiscInput : uint16_t
    {
        IN_NONE       = 0 << 0,
        IN_ADC_DO     = 1 << 0,
        IN_ADC_SARS   = 1 << 1,
        IN_CART_SDA   = 1 << 2,
        IN_JVS_SENSE  = 1 << 3,
        IN_JVS_IRDY   = 1 << 4,
        IN_JVS_DRDY   = 1 << 5,
        IN_CART_IRDY  = 1 << 6,
        IN_CART_DRDY  = 1 << 7,
        IN_COIN1      = 1 << 8,
        IN_COIN2      = 1 << 9,
        IN_PCMCIA_CD1 = 1 << 10,
        IN_PCMCIA_CD2 = 1 << 11,
        IN_SERVICE    = 1 << 12
    };

    ENABLE_BITWISE_OPS(MiscInput);

    void SetOutputs(MiscOutput state);                          // Writes all outputs to the raw given data
    void SetOutputsMasked(MiscOutput outputs, MiscOutput mask); // Writes multiple bits using a mask to only change select bits
    void SetOutputBit(MiscOutput output, bool state);           // Sets/Unsets a single bit of the output data
    void PulseOutput(MiscOutput output);                        // Pulses output (only supports coin counters and JVS reset, blocking)
    void MuteAudio(MiscOutput channel);                         // Mutes the passed audio channels (only supports audio channels)
    void UnmuteAudio(MiscOutput channel);                       // Unmutes the passed audio channels (only supports audio channels)

    MiscOutput GetOutputs(); // Returns the state of the output port (stored in RAM)
} // namespace System573::IO::ASIC