/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * registers.hpp - Created on 24-02-2026
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

#include "psx/common/registers.hpp"
#include <cstdint>
namespace PS1::IO::SIO
{
    enum StatusFlag : uint16_t
    {
        STAT_TX_NOT_FULL   = 1 << 0,
        STAT_RX_NOT_EMPTY  = 1 << 1,
        STAT_TX_EMPTY      = 1 << 2,
        STAT_RX_PARITY_ERR = 1 << 3,
        STAT_RX_OVERRUN    = 1 << 4, // SIO1 only
        STAT_RX_STOP_ERR   = 1 << 5, // SIO1 only
        STAT_RX_INVERT     = 1 << 6, // SIO1 only
        STAT_DSR           = 1 << 7, // DSR is /ACK on SIO0
        STAT_CTS           = 1 << 8, // SIO1 only
        STAT_IRQ           = 1 << 9
    };

    enum ModeFlag : uint16_t
    {
        MODE_BAUD_BITMASK   = 3 << 0,
        MODE_BAUD_DIV1      = 1 << 0,
        MODE_BAUD_DIV16     = 2 << 0,
        MODE_BAUD_DIV64     = 3 << 0,
        MODE_DATA_BITMASK   = 3 << 2,
        MODE_DATA_5         = 0 << 2,
        MODE_DATA_6         = 1 << 2,
        MODE_DATA_7         = 2 << 2,
        MODE_DATA_8         = 3 << 2,
        MODE_PARITY_BITMASK = 3 << 4,
        MODE_PARITY_NONE    = 0 << 4,
        MODE_PARITY_EVEN    = 1 << 4,
        MODE_PARITY_ODD     = 3 << 4,
        MODE_STOP_BITMASK   = 3 << 6, // SIO1 only
        MODE_STOP_1         = 1 << 6, // SIO1 only
        MODE_STOP_1_5       = 2 << 6, // SIO1 only
        MODE_STOP_2         = 3 << 6, // SIO1 only
        MODE_SCK_INVERT     = 1 << 8  // SIO0 only
    };

    enum ControlFlag : uint16_t
    {
        CTRL_TX_ENABLE      = 1 << 0,
        CTRL_DTR            = 1 << 1, // DTR is /CS on SIO0
        CTRL_RX_ENABLE      = 1 << 2,
        CTRL_TX_INVERT      = 1 << 3, // SIO1 only
        CTRL_ACKNOWLEDGE    = 1 << 4,
        CTRL_RTS            = 1 << 5, // SIO1 only
        CTRL_RESET          = 1 << 6,
        CTRL_TX_IRQ_ENABLE  = 1 << 10,
        CTRL_RX_IRQ_ENABLE  = 1 << 11,
        CTRL_DSR_IRQ_ENABLE = 1 << 12, // DSR is /ACK on SIO0
        CTRL_CS_PORT_1      = 0 << 13, // SIO0 only
        CTRL_CS_PORT_2      = 1 << 13  // SIO0 only
    };

    // SIO_DATA is a 32-bit register, but some emulators do not implement it
    // correctly and break if it's read more than 8 bits at a time.
    // #define SIO_DATA(N) _MMIO8((IO_BASE | 0x040) + (16 * (N)))
    // #define SIO_STAT(N) _MMIO16((IO_BASE | 0x044) + (16 * (N)))
    // #define SIO_MODE(N) _MMIO16((IO_BASE | 0x048) + (16 * (N)))
    // #define SIO_CTRL(N) _MMIO16((IO_BASE | 0x04a) + (16 * (N)))
    // #define SIO_BAUD(N) _MMIO16((IO_BASE | 0x04e) + (16 * (N)))

    inline volatile uint8_t &DATA(int n)
    {
        return *reinterpret_cast<volatile uint8_t *>((IO_BASE | 0x040) + (16 * n));
    }

    inline volatile uint16_t &STAT(int n)
    {
        return *reinterpret_cast<volatile uint16_t *>((IO_BASE | 0x044) + (16 * n));
    }

    inline volatile uint16_t &MODE(int n)
    {
        return *reinterpret_cast<volatile uint16_t *>((IO_BASE | 0x048) + (16 * n));
    }

    inline volatile uint16_t &CTRL(int n)
    {
        return *reinterpret_cast<volatile uint16_t *>((IO_BASE | 0x04A) + (16 * n));
    }

    inline volatile uint16_t &BAUD(int n)
    {
        return *reinterpret_cast<volatile uint16_t *>((IO_BASE | 0x04E) + (16 * n));
    }
} // namespace PS1::IO::SIO