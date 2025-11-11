/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * psx_sio0.hpp - Created on 09-11-2025
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
#include <stddef.h>

#include "psx/registers.hpp"

namespace System::PSX::IO
{
    enum Address : uint8_t
    {
        ADDR_CONTROLLER   = 0x01,
        ADDR_PS2_IR       = 0x21,
        ADDR_PS2_MULTITAP = 0x61,
        ADDR_MEMORY_CARD  = 0x81
    };

    enum ResponsePrefix : uint8_t
    {
        PREFIX_CONTROLLER  = 0x5a,
        PREFIX_MEMORY_CARD = 0x5d
    };

    enum Command : uint8_t
    {
        // Basic controller commands
        CMD_POLL   = 'B',
        CMD_CONFIG = 'C',

        // Configuration mode commands
        CMD_INIT_PRESSURE = '@', // DualShock 2 only
        CMD_RESP_INFO     = 'A', // DualShock 2 only
        CMD_SET_ANALOG    = 'D',
        CMD_GET_ANALOG    = 'E',
        CMD_MOTOR_INFO    = 'F',
        CMD_MOTOR_LIST    = 'G',
        CMD_MOTOR_STATE   = 'H',
        CMD_GET_MODES     = 'L',
        CMD_REQ_CONFIG    = 'M',
        CMD_RESP_CONFIG   = 'O', // DualShock 2 only

        // Memory card commands
        CMD_READ_SECTOR   = 'R',
        CMD_IDENTIFY_CARD = 'S', // OEM cards only
        CMD_WRITE_SECTOR  = 'W',

        CMD_AFFIX_MULTITAP_ALL = 0x01, // Tells the multitap to read and respond all four pads
    };

    enum SIOState : uint8_t
    {
        SIO0_OKAY,
        SIO0_IN_USE,
        SIO0_NO_RESPONSE,
    };

    class PSX_SIO0
    {
        friend class PSX_Joypad;
        friend class PSX_MemCard;
        friend class SIO0_Transaction;

    private:
        bool _initialised = false;
        int _initResult   = 0;
        bool _inUse       = false;

        int init_();
        int start_(uint8_t address, SIOControlFlag port);
        void stop_();
        uint8_t exchangeByte_(uint8_t value);
        size_t exchangeBytes_(
            const uint8_t *request,
            uint8_t *response,
            size_t reqLength,
            size_t maxRespLength,
            bool hasLastACK = false);

        void mouseFix_();
    };

    extern PSX_SIO0 psx_sio0;
} // namespace System::PSX::IO