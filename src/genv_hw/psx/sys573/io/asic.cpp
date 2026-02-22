/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * asic.c - Created on 26-11-2025
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

#include "asic.hpp"

#include "psx/common/system/sys.h"
#include "psx/sys573/registers573.hpp"

#define OUTPUT_PULSE_TIME 20

namespace System573::IO::ASIC
{
    // Used to keep track of the output port state
    MiscOutput asicOutState = MiscOutput::NONE;

    void writeOutputs(MiscOutput state)
    {
        asicOutState    = state;
        SYS573_MISC_OUT = static_cast<uint16_t>(asicOutState);
    }

    void writeOutputsMasked(MiscOutput outputs, MiscOutput mask)
    {
        auto asic = asicOutState & ~(mask);
        outputs   = (outputs & mask);
        writeOutputs(asic | outputs);
    }

    void writeOutputBit(MiscOutput output, bool state)
    {
        auto asic = asicOutState;
        if (state)
            asic |= output;
        else
            asic &= ~output;
        writeOutputs(asic);
    }

    MiscOutput getOutputs()
    {
        return asicOutState;
    }

    void pulseOutput(MiscOutput output)
    {
        output &= (MiscOutput::COIN_COUNT1 | MiscOutput::COIN_COUNT1 | MiscOutput::JVS_RESET);
        if (output == MiscOutput::NONE) return;

        writeOutputs(asicOutState |= output);
        psx_delayMicrosecondsBusy(OUTPUT_PULSE_TIME);
        writeOutputs(asicOutState &= ~output);
    }

    void muteAudio(MiscOutput channel)
    {
        channel &= (MiscOutput::SPU_ENABLE | MiscOutput::AMP_ENABLE | MiscOutput::CDDA_ENABLE);
        if (channel == MiscOutput::NONE) return;
        writeOutputs(asicOutState &= ~channel);
    }

    void unmuteAudio(MiscOutput channel)
    {
        channel |= (MiscOutput::SPU_ENABLE | MiscOutput::AMP_ENABLE | MiscOutput::CDDA_ENABLE);
        if (channel == MiscOutput::NONE) return;
        writeOutputs(asicOutState |= channel);
    }
} // namespace System573::IO::ASIC