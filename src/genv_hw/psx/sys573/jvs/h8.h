/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * h8.hpp - Created on 25-11-2025
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

#include "../registers573.hpp"
#include "../asic.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        H8_STATUS_WAITING = 0,
        H8_STATUS_BUSY,
        H8_STATUS_TRANSFER_IN_PROGRESS,
        H8_INVALID_STATUS_CODE
    } H8Status;

    typedef enum
    {
        H8_NO_ERROR = 0,
        H8_ERROR_INVALID_CHECKSUM,
        H8_ERROR_INVALID_SYNC,
        H8_INVALID_ERROR_CODE,
    } H8Error;

    // Acknowledges the incoming data has been read by the CPU from the H8 MCU
    // Once the data has been ack'd, the H8 will place the next byte of data on
    // the single stage FIFO
    inline void sys573_h8_ack()
    {
        SYS573_JVS_IRDY_ACK = 0;
    }

    // Gets the current status code from the H8
    inline uint8_t sys573_h8_status()
    {
        return ((SYS573_DIP_CART & 0x0C) >> 2);
    }

    // Gets the current error code from the H8
    inline uint8_t sys573_h8_error()
    {
        return ((SYS573_DIP_CART & 0x30) >> 4);
    }

    // Returns true if the JVS Sense input pin is being asserted.
    // JVS says the sense pin must be 2.5v when an IO board is connected but not
    // set with an ID, and 0v when ID has been given. If this function returns false,
    // an IO board is not present.
    inline bool sys573_h8_sense_in()
    {
        return !(SYS573_MISC_IN & SYS573_MISC_IN_JVS_SENSE);
    }

    // Resets the H8 using the ASIC pulse function
    inline void sys573_h8_reset()
    {
        sys573_asic_pulse_output(SYS573_MISC_OUT_JVS_RESET);
    }

#ifdef __cplusplus
}
#endif