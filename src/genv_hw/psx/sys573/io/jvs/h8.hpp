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

#include "psx/sys573/registers573.hpp"
#include "psx/sys573/io/asic.hpp"

namespace System573::IO::H8
{
    typedef enum
    {
        H8_STATUS_WAITING              = 0,
        H8_STATUS_BUSY                 = 1,
        H8_STATUS_TRANSFER_IN_PROGRESS = 2,
        H8_INVALID_STATUS_CODE         = 3
    } H8Status;

    typedef enum
    {
        H8_NO_ERROR               = 3,
        H8_ERROR_INVALID_CHECKSUM = 1,
        H8_ERROR_INVALID_SYNC     = 2,
        H8_INVALID_ERROR_CODE     = 4,
    } H8Error;

    // Acknowledges the incoming data has been read by the CPU from the H8 MCU
    // Once the data has been ack'd, the H8 will place the next byte of data on
    // the single stage FIFO
    inline void ack()
    {
        SYS573_JVS_IRDY_ACK = 0;
    }

    inline bool txReady()
    {
        return (static_cast<MiscInput>(SYS573_MISC_IN) & MiscInput::JVS_DRDY) != MiscInput::NONE;
    }

    inline bool available()
    {
        return (static_cast<MiscInput>(SYS573_MISC_IN) & MiscInput::JVS_IRDY) != MiscInput::NONE;
    }

    inline bool writeWord(uint16_t word)
    {
        if (txReady())
        {
            SYS573_JVS_TX_DATA = word;
            return true;
        }
        return false;
    }

    // Gets the current status code from the H8
    inline H8Status status()
    {
        return static_cast<H8Status>((SYS573_DIP_CART & 0x0C) >> 2);
    }

    // Gets the current error code from the H8
    inline H8Error error()
    {
        return static_cast<H8Error>((SYS573_DIP_CART & 0x30) >> 4);
    }

    // Returns true if the JVS Sense input pin is being asserted.
    // JVS says the sense pin must be 2.5v when an IO board is connected but not
    // set with an ID, and 0v when ID has been given. If this function returns false,
    // an IO board is not present.
    inline bool sense_in()
    {
        return (static_cast<MiscInput>(SYS573_MISC_IN) & MiscInput::JVS_SENSE) != MiscInput::NONE;
    }

    // Resets the H8 using the ASIC pulse function
    inline void reset()
    {
        ASIC::pulseOutput(MiscOutput::JVS_RESET);
    }

} // namespace System573::IO::H8