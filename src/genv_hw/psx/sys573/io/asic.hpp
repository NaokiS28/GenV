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

#include "psx/sys573/registers573.hpp"
#include <stdint.h>
#include <stdbool.h>

namespace System573::IO::ASIC
{
    void writeOutputs(MiscOutput state);                          // Writes all outputs to the raw given data
    void writeOutputsMasked(MiscOutput outputs, MiscOutput mask); // Writes multiple bits using a mask to only change select bits
    void writeOutputBit(MiscOutput output, bool state);           // Sets/Unsets a single bit of the output data
    void pulseOutput(MiscOutput output);                          // Pulses output (only supports coin counters and JVS reset, blocking)
    void muteAudio(MiscOutput channel);                           // Mutes the passed audio channels (only supports audio channels)
    void unmuteAudio(MiscOutput channel);                         // Unmutes the passed audio channels (only supports audio channels)

    MiscOutput get_outputs(); // Returns the state of the output port (stored in RAM)
} // namespace System573::IO::ASIC