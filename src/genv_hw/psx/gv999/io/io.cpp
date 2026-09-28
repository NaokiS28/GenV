/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * io.cpp - Created on 26-11-2025
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

#include "psx/common/system/sys.h"

#define OUTPUT_PULSE_TIME 20

namespace SystemGV::IO
{
    // Used to keep track of the output port state
    Outputs asicOutState = Outputs::NONE;

    void SetOutputs(Outputs state)
    {
        asicOutState  = state;
        Regs::Outputs = (uint16_t)asicOutState;
    }

    void SetOutputsMasked(Outputs outputs, Outputs mask)
    {
        auto asic = asicOutState & ~(mask);
        outputs   = (outputs & mask);
        SetOutputs(asic | outputs);
    }

    void SetOutputBit(Outputs output, bool state)
    {
        auto asic = asicOutState;
        if (state)
            asic |= output;
        else
            asic &= ~output;
        SetOutputs(asic);
    }

    Outputs GetOutputs()
    {
        return asicOutState;
    }

    void PulseOutput(Outputs output)
    {
        output &= (Outputs::COIN_COUNTER_1 | Outputs::COIN_COUNTER_2);
        if (output == Outputs::NONE) return;

        SetOutputs(asicOutState |= output);
        psx_delayMicrosecondsBusy(OUTPUT_PULSE_TIME);
        SetOutputs(asicOutState &= ~output);
    }

    void MuteAudio()
    {
        // channel &= (OUT_SPU_ENABLE | OUT_AMP_ENABLE | OUT_CDDA_ENABLE);
        // if (channel == OUT_NONE) return;
        // SetOutputs(asicOutState &= ~channel);
    }

    void unMuteAudio()
    {
        // channel |= (OUT_SPU_ENABLE | OUT_AMP_ENABLE | OUT_CDDA_ENABLE);
        // if (channel == OUT_NONE) return;
        // SetOutputs(asicOutState |= channel);
    }
} // namespace SystemGV::IO