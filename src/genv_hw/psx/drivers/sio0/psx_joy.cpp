/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * psx_joy.cpp - Created on 23-08-2025
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

#include "psx_joy.hpp"

#include "common/return_codes.hpp"
#include "psx/drivers/sio0/psx_sio0.hpp"
#include "psx/registers.hpp"
#include "psx/system/sys.h"
#include "common/logger/log.hpp"

#define LOG_JOY(fmt, ...) LOG("psx_joy", fmt __VA_OPT__(, ) __VA_ARGS__)
#define START(addr, port)                                    \
    {                                                        \
        int r = 0;                                           \
        if (r = psx_sio0.start_(addr, port), r != SIO0_OKAY) \
        {                                                    \
            switch (r)                                       \
            {                                                \
            case SIO0_IN_USE: return 3;                      \
            case SIO0_NO_RESPONSE: return 1;                 \
            }                                                \
        }                                                    \
    }

#define END(data, rspLen)                 \
    {                                     \
        psx_sio0.stop_();                 \
        if (rspLen < 4) return 1;         \
        if (data[1] != PREFIX_CONTROLLER) \
            return 2;                     \
    }

namespace System::PSX::IO
{
    uint8_t PSX_Joypad::driverCount = 0;

    int PSX_Joypad::update()
    {
        if (poll_() == GV_OK)
        {
            // Do shit
        }
        return 0;
    }

    // Enter or exit controller config mode. Only works on DualShock and above
    int PSX_Joypad::configMode_(bool state)
    {
        uint8_t request[4]{CMD_CONFIG, 0, state, 0};
        uint8_t response[8];

        START(ADDR_CONTROLLER, _portNumber);
        size_t respLength = psx_sio0.exchangeBytes_(
            request,
            response,
            sizeof(request),
            sizeof(response));
        END(response, respLength);

        return 0;
    }

    // Enables setting analog button and locking it. Only works on DualShock and above
    int PSX_Joypad::setAnalog_(bool state, bool lock)
    {
        const uint8_t request[4]{CMD_SET_ANALOG, 0, state, (uint8_t)(lock ? 0x3 : 0x0)};
        uint8_t response[8];

        START(ADDR_CONTROLLER, _portNumber);
        size_t respLength = psx_sio0.exchangeBytes_(
            request,
            response,
            sizeof(request),
            sizeof(response));
        END(response, respLength);

        return 0;
    }

    // Enables dual motors. Only works on DualShock and above
    int PSX_Joypad::setDualshock_(bool state)
    {
        const uint8_t request[]{CMD_REQ_CONFIG, 0, 0, state, 0xFF, 0xFF, 0xFF, 0xFF};
        uint8_t response[8];

        START(ADDR_CONTROLLER, _portNumber);
        size_t respLength = psx_sio0.exchangeBytes_(
            request,
            response,
            sizeof(request),
            sizeof(response));
        END(response, respLength);

        return 0;
    }

    // Enables reading analog buttons. Only works on DualShock2
    // Configures how the normal controller response sequence works
    int PSX_Joypad::setDS2Analog_(uint32_t bitmask)
    {
        uint8_t mask[] = {
            (uint8_t)(bitmask & 0xFF),
            (uint8_t)((bitmask >> 8) & 0xFF),
            (uint8_t)((bitmask >> 16) & 0xFF),
            (uint8_t)((bitmask >> 24) & 0xFF),
            0};
        const uint8_t request[]{CMD_RESP_CONFIG, 0, mask[0], mask[1], mask[2], mask[3], mask[4]};
        uint8_t response[8];

        START(ADDR_CONTROLLER, _portNumber);
        size_t respLength = psx_sio0.exchangeBytes_(
            request,
            response,
            sizeof(request),
            sizeof(response));
        END(response, respLength);

        return 0;
    }

    int PSX_Joypad::poll_(void)
    {

        // auto lastType  = controllerType;
        // controllerType = TYPE_NONE;
        // buttons        = 0;
        // leftAnalog.x   = 0;
        // leftAnalog.y   = 0;
        // rightAnalog.x  = 0;
        // rightAnalog.y  = 0;

        const uint8_t request[4]{CMD_POLL, 0, 0, 0};
        uint8_t response[8];

        START(ADDR_CONTROLLER, _portNumber);
        size_t respLength = psx_sio0.exchangeBytes_(
            request,
            response,
            sizeof(request),
            sizeof(response));
        END(response, respLength);

        // controllerType = ControllerType(response[0] >> 4);
        // buttons        = ~util::concat2(response[2], response[3]);

        // The PS1 mouse outputs signed motion deltas while all other controllers
        // use unsigned values.
        // int offset = (controllerType == TYPE_MOUSE) ? 0 : 128;

        // if (respLength >= 6)
        //{
        //    rightAnalog.y = response[4] - offset;
        //    rightAnalog.x = response[5] - offset;
        //}
        // if (respLength >= 8)
        //{
        //    leftAnalog.y = response[6] - offset;
        //    leftAnalog.x = response[7] - offset;
        //}

        // return (controllerType == lastType) ? NO_ERROR : DEVICE_CHANGED;
        return 0;
    }
} // namespace System::PSX::IO