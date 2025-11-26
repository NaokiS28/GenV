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

#include "asic.h"

#include "psx/psx/system/sys.h"
#include "psx/sys573/registers573.hpp"

#define OUTPUT_PULSE_TIME 20

// Used to keep track of the output port state
uint8_t sys573_asic_out = 0;

void sys573_asic_set_outputs(uint16_t data)
{
    sys573_asic_out = data;
    SYS573_MISC_OUT = sys573_asic_out;
}

void sys573_asic_write_output_bit(uint8_t bit, bool state)
{
    if (bit > 8) return;
    if (state)
        sys573_asic_out |= (state << bit);
    else
        sys573_asic_out &= ~(state << bit);
    SYS573_MISC_OUT = sys573_asic_out;
}

uint8_t sys573_asic_get_outputs()
{
    return sys573_asic_out;
}

void sys573_asic_pulse_output(uint16_t output)
{
    output &= (SYS573_MISC_OUT_COIN_COUNT1 | SYS573_MISC_OUT_COIN_COUNT1 | SYS573_MISC_OUT_JVS_RESET);
    if (!output) return;

    sys573_asic_out |= output;
    SYS573_MISC_OUT = sys573_asic_out;
    psx_delayMicrosecondsBusy(OUTPUT_PULSE_TIME);
    sys573_asic_out &= ~output;
    SYS573_MISC_OUT = sys573_asic_out;
}

void sys573_asic_mute_audio(uint16_t channel)
{
    channel &= (SYS573_MISC_OUT_SPU_ENABLE | SYS573_MISC_OUT_AMP_ENABLE | SYS573_MISC_OUT_CDDA_ENABLE);
    if (!channel) return;

    sys573_asic_out &= ~channel;
    SYS573_MISC_OUT = sys573_asic_out;
}

void sys573_asic_unmute_audio(uint16_t channel)
{
    channel |= (SYS573_MISC_OUT_SPU_ENABLE | SYS573_MISC_OUT_AMP_ENABLE | SYS573_MISC_OUT_CDDA_ENABLE);
    if (!channel) return;

    sys573_asic_out |= channel;
    SYS573_MISC_OUT = sys573_asic_out;
}