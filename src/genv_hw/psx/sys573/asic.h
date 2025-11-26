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

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void sys573_asic_set_outputs(uint16_t data);                // Writes all outputs to the raw given data
    void sys573_asic_write_output_bit(uint8_t bit, bool state); // Sets/Unsets a single bit of the output data
    void sys573_asic_pulse_output(uint16_t output);             // Pulses output (only supports coin counters and JVS reset, blocking)
    void sys573_asic_mute_audio(uint16_t channel);              // Mutes the passed audio channels (only supports audio channels)
    void sys573_asic_unmute_audio(uint16_t channel);            // Unmutes the passed audio channels (only supports audio channels)

    uint8_t sys573_asic_get_outputs(); // Returns the state of the output port (stored in RAM)

#ifdef __cplusplus
}
#endif