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
    enum StatusFlag : uint8_t
    {
        STATUS_WAITING              = 0,
        STATUS_BUSY                 = 1,
        STATUS_TRANSFER_IN_PROGRESS = 2,
        INVALID_STATUS_CODE         = 3
    };

    enum ErrorFlag : uint8_t
    {
        NO_ERROR               = 3,
        ERROR_INVALID_CHECKSUM = 1,
        ERROR_INVALID_SYNC     = 2,
        INVALID_ERROR_CODE     = 4,
    };

    // Acknowledges the incoming data has been read by the CPU from the H8 MCU
    // Once the data has been ack'd, the H8 will place the next byte of data on
    // the single stage FIFO
    inline void Ack()
    {
        ASIC::Regs::JvsIrdyAck = 0;
    }

    inline bool TxReady()
    {
        return (ASIC::Regs::MiscIn & ASIC::IN_JVS_DRDY) != ASIC::IN_NONE;
    }

    inline bool Available()
    {
        return (ASIC::Regs::MiscIn & ASIC::IN_JVS_IRDY) != ASIC::IN_NONE;
    }

    inline bool WriteWord(uint16_t word)
    {
        if (TxReady())
        {
            ASIC::Regs::JvsTxData = word;
            return true;
        }
        return false;
    }

    // Gets the current status code from the H8
    inline StatusFlag Status()
    {
        return static_cast<StatusFlag>((ASIC::Regs::DipCart & 0x0C) >> 2);
    }

    // Gets the current error code from the H8
    inline ErrorFlag Error()
    {
        return static_cast<ErrorFlag>((ASIC::Regs::DipCart & 0x30) >> 4);
    }

    // Resets the H8 using the ASIC pulse function
    inline void Reset()
    {
        ASIC::PulseOutput(ASIC::OUT_JVS_RESET);
    }

} // namespace System573::IO::H8