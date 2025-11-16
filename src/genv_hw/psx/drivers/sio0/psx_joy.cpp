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

#include <string.h>

#include "common/return_codes.hpp"
#include "psx/drivers/sio0/psx_sio0.hpp"
#include "psx/registers.hpp"
#include "psx/system/sys.h"
#include "common/logger/log.hpp"

#define LOG_JOY(fmt, ...) LOG("psx_joy", fmt __VA_OPT__(, ) __VA_ARGS__)
#define START(addr, port)                                   \
    {                                                       \
        int r = 0;                                          \
        if (r = psx_sio0.start(addr, port), r != SIO0_OKAY) \
        {                                                   \
            switch (r)                                      \
            {                                               \
            case SIO0_IN_USE: return 3;                     \
            case SIO0_NO_RESPONSE: return 1;                \
            }                                               \
        }                                                   \
    }

#define END(data, rspLen)                 \
    {                                     \
        psx_sio0.stop();                  \
        if (rspLen < 4) return 1;         \
        if (data[1] != PREFIX_CONTROLLER) \
            return 2;                     \
    }

#define BOOL(val) ((val) > 0 ? 1 : 0)

namespace System::PSX::IO
{
    uint8_t PSX_Joypad::driverCount = 0;

    struct ControllerReadResponse
    {
        union
        {
            uint8_t idLo;
            uint8_t idHi;
            uint16_t id;
        };
        uint16_t input;
        struct AnalogInput
        {
            uint8_t x, y;
        } left, right;
    };

    void printPSControlDebug(ControllerReadResponse &resp)
    {
        switch (resp.idLo)
        {
        case PAD_DIGITAL: LOG_JOY("Digital, DATA: %04X", resp.input); return;
        case PAD_ANALOG:
            LOG_JOY("Analog/DS: Buttons: %04X, Lx:%d Ly:%d Rx:%d Ry:%d",
                    resp.input,
                    (int8_t)resp.left.x,
                    (int8_t)resp.left.y,
                    (int8_t)resp.right.x,
                    (int8_t)resp.left.y);
            return;
        case PAD_GUNCON:
        {
            const char *err = "OK";
            if (resp.left.x == 0x01)
            {
                switch (resp.left.y)
                {
                case 0x5: err = "Unexp. Light"; break;
                case 0xA: err = "No Light"; break;
                default: break;
                }
            }
            LOG_JOY(
                "GunCon: A:%x B:%x T:%x x:%u y:%u Error: %s",
                BOOL(resp.input & BTN_GUNCON_A),
                BOOL(resp.input & BTN_GUNCON_B),
                BOOL(resp.input & BTN_GUNCON_TRIGGER),
                resp.left.x,
                resp.left.y,
                err);
        }
        case PAD_MOUSE:
            LOG_JOY(
                "Mouse: Left:%x, Right:%x x:%d y:%d",
                BOOL(resp.input & BTN_MOUSE_LEFT),
                BOOL(resp.input & BTN_MOUSE_RIGHT),
                (int8_t)resp.left.x,
                (int8_t)resp.left.y);
            return;
        default: LOG_JOY("ID: %02X, DATA: %04X", resp.idLo, resp.input); return;
        }
    }

    int PSX_Joypad::update()
    {
        psx_sio0.update(); // Mouse ack checking

        int fr = GV_OK;
        for (auto pad : _padList)
        {
            switch (poll(pad.subBusID))
            {
            case GV_OK: continue;
            case SIO0_NO_RESPONSE: psx_sio0.setMultitapState(_portNumber, MT_TEST_PRESENCE); continue;
            default: break; // If SIO0 is in use (somehow on a single threaded app..) ignore.
            }

            if (!psx_sio0.multitapPresent(_portNumber)) break; // No need to scan further
        }

        return (fr == GV_OK ? 0 : 1);
    }

    bool PSX_Joypad::reset()
    {
        for (auto pad : _padList)
        {
            Services::dettachInputDevice(&pad);
        }
        memset(_padList, 0, sizeof(IInputDevice));
        memset(_padData, 0, sizeof(PSX_PadData));
        return true;
    }

    // Enter or exit controller config mode. Only works on DualShock and above
    int PSX_Joypad::configMode_(bool state)
    {
        uint8_t request[4]{CMD_CONFIG, 0, state, 0};
        uint8_t response[8];

        START(ADDR_CONTROLLER, _portNumber);
        size_t respLength = psx_sio0.exchangeBytes(
            request,
            response,
            sizeof(request),
            sizeof(response));
        END(response, respLength);

        return GV_OK;
    }

    // Enables setting analog button and locking it. Only works on DualShock and above
    int PSX_Joypad::setAnalog_(bool state, bool lock)
    {
        const uint8_t request[4]{CMD_SET_ANALOG, 0, state, (uint8_t)(lock ? 0x3 : 0x0)};
        uint8_t response[8];

        START(ADDR_CONTROLLER, _portNumber);
        size_t respLength = psx_sio0.exchangeBytes(
            request,
            response,
            sizeof(request),
            sizeof(response));
        END(response, respLength);

        return GV_OK;
    }

    // Enables dual motors. Only works on DualShock and above
    int PSX_Joypad::setDualshock_(bool state)
    {
        const uint8_t request[]{CMD_REQ_CONFIG, 0, 0, state, 0xFF, 0xFF, 0xFF, 0xFF};
        uint8_t response[8];

        START(ADDR_CONTROLLER, _portNumber);
        size_t respLength = psx_sio0.exchangeBytes(
            request,
            response,
            sizeof(request),
            sizeof(response));
        END(response, respLength);

        return GV_OK;
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
        size_t respLength = psx_sio0.exchangeBytes(
            request,
            response,
            sizeof(request),
            sizeof(response));
        END(response, respLength);

        return GV_OK;
        return GV_OK;
    }

    // Polls a standard controller on given subport (of multitap)
    int PSX_Joypad::poll(uint8_t subport)
    {
        // Send a poll command. Also send a multitap enable command. If MT is not present, controller will respond
        // and we detect this and turn of this test. When we get NO_ACK from the SIO0 driver, start testing again.
        // Theory is that after NO_ACKs, controller was unplugged and that whilst we still have a controller plugged in,
        // if it responded as a multitap, then we can assume it's always one.
        // We use the second method of polling when using a multitap so it's more flexible, just using method 1 to test presence.

        MultitapState _mt = psx_sio0.getMultitapState(_portNumber);

        const uint8_t request[4]{CMD_POLL, (uint8_t)(_mt == MT_TEST_PRESENCE ? 1 : 0), 0, 0};
        alignas(ControllerReadResponse) uint8_t response[(2 * (4 * 4))];

        START(ADDR_CONTROLLER + subport, _portNumber);
        size_t respLength = psx_sio0.exchangeBytes(
            request,
            response,
            sizeof(request),
            sizeof(response));
        END(response, respLength);

        static ControllerReadResponse lastResp;
        auto &resp = *reinterpret_cast<ControllerReadResponse *>(response);

        if (_mt == MT_TEST_PRESENCE && resp.idLo == PAD_MULTITAP)
        {
            psx_sio0.setMultitapState(_portNumber, MT_IS_PRESENT);
            resp = *reinterpret_cast<ControllerReadResponse *>(response[2]); // Port A data
        }
        else
        {
            psx_sio0.setMultitapState(_portNumber, MT_NOT_PRESENT);
        }

        if (memcmp(&lastResp, &resp, sizeof(ControllerReadResponse)))
        {
            printPSControlDebug(resp);
            lastResp = resp;
        }

        return GV_OK;
    }
} // namespace System::PSX::IO