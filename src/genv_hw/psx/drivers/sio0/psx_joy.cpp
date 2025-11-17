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
#include "common/services/io/iface_input.hpp"
#include "common/services/services.hpp"
#include "psx/drivers/sio0/psx_pads.hpp"
#include "psx/drivers/sio0/psx_sio0.hpp"
#include "psx/psx_strings.hpp"
#include "psx/registers.hpp"
#include "psx/system/sys.h"
#include "common/logger/log.hpp"

#define LOG_JOY(fmt, ...) LOG("psx_joy", fmt __VA_OPT__(, ) __VA_ARGS__)
#define START(addr, port)                                   \
    {                                                       \
        int r = 0;                                          \
        if (r = psx_sio0.start(addr, port), r != SIO0_OKAY) \
        {                                                   \
            return r;                                       \
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

    IInputDevice addController(
        ControllerReadResponse &resp,
        PlayerSuggestion player,
        uint8_t subport,
        uint32_t *digital,
        int16_t *analog)
    {
        switch (resp.idLo)
        {
        case PAD_DIGITAL: return psxPad(PSX_DIGITAL_STR, PSX_DIGITAL_HASH, player, subport, digital, 14);
        case PAD_ANALOG: return psxPad(PSX_ANALOG_STR, PSX_ANALOG_HASH, player, subport, digital, 16, analog, 4);
        case PAD_DUALSHOCK2: return psxPad(PSX_DUALSHOCK2_STR, PSX_DUALSHOCK2_HASH, player, subport, digital, 16, analog, 10);
        case PAD_TWINSTICK: return psxPad(PSX_TWINSTICK_STR, PSX_TWINSTICK_HASH, player, subport, digital, 14, analog, 4);
        case PAD_GUNCON: return psxGun(PSX_GUNCON_STR, PSX_GUNCON_HASH, player, subport, digital, analog);
        case PAD_KONAMI_GUN: return psxGun(PSX_JUSTIFIER_STR, PSX_JUSTIFIER_HASH, player, subport, digital, analog);
        // case PAD_MOUSE: return devMouse(player, subport, digital, delta);
        // case PAD_KEYBOARD: break;
        case PAD_NEGCON: return psxPad(PSX_NEGCON_STR, PSX_NEGCON_HASH, player, subport, digital, 7, analog, 4);
        // case PAD_JOGCON: return psxJogcon(player, subport, digital, delta);
        default: return IInputDevice();
        }
    }

    constexpr const char *getPadName(uint16_t id)
    {
        switch (id & 0xFF)
        {
        case PAD_DIGITAL: return PSX_DIGITAL_STR;
        case PAD_ANALOG: return PSX_ANALOG_STR;
        case PAD_DUALSHOCK2: return PSX_DUALSHOCK2_STR;
        case PAD_TWINSTICK: return PSX_TWINSTICK_STR;
        case PAD_GUNCON: return PSX_GUNCON_STR;
        case PAD_KONAMI_GUN: return PSX_JUSTIFIER_STR;
        // case PAD_MOUSE: return PSX_MOUSE_STR;
        // case PAD_KEYBOARD: return PSX_KEYBOARD_STR;
        case PAD_NEGCON: return PSX_NEGCON_STR;
        // case PAD_JOGCON: return PSX_JOGCON_STR;
        default: return "Unsupported controller";
        }
    }

    int PSX_Joypad::update()
    {
        psx_sio0.update(); // Mouse ack checking

        int fr   = GV_OK;
        int port = 0;
        ControllerReadResponse resp;
        // Will always do the first subport (assuming multitap is connected, else just first port)
        for (auto &pad : _padList)
        {
            switch (poll(resp, pad.subBusID))
            {
            case SIO0_OKAY:
                if (_padData[pad.subBusID].type == PAD_DISCONNECTED ||
                    _padData[pad.subBusID].type != resp.idLo)
                {
                    // Device changed
                    LOG("psxpad", "Controller changed on port %d to 0x%04X (%s)",
                        (_portNumber == SIO_CTRL_CS_PORT_1 ? 1 : 2),
                        resp.id,
                        getPadName(resp.id));

                    if (_padData[pad.subBusID].doDSTest)
                    {
                        _padData[pad.subBusID].doDSTest = false;
                        // Config mode only works on DualShock and above
                        if (!configMode_(true, pad.subBusID))
                        {
                            setAnalog_(true, false, pad.subBusID);
                            setDualshock_(true, pad.subBusID);
                            setDS2Analog_(0x3FFFF, pad.subBusID);
                            configMode_(false, pad.subBusID);
                            poll(resp, pad.subBusID);

                            LOG("psxpad", "Set controller mode on port %d to 0x%04X (%s)",
                                (_portNumber == SIO_CTRL_CS_PORT_1 ? 1 : 2),
                                resp.id,
                                getPadName(resp.id));
                        }
                    }

                    _padData[pad.subBusID].type = static_cast<JoypadType>(resp.idLo);

                    pad = addController(
                        resp, Input::DEVICE_PLAYER_1, pad.subBusID,
                        &_padData[pad.subBusID].digital,
                        _padData[pad.subBusID].analog);
                    if (pad.type != Input::DEVICE_TYPE_NULL)
                        Services::attachInputDevice(&pad);
                }

                break;
            case SIO0_NO_RESPONSE:
                if (_padData[pad.subBusID].type != PAD_DISCONNECTED)
                {
                    LOG("psxpad", "Controller disconnected on port %d", (_portNumber == SIO_CTRL_CS_PORT_1 ? 1 : 2));
                    if (pad.type != Input::DEVICE_TYPE_NULL)
                        Services::dettachInputDevice(&pad);
                    _padData[pad.subBusID] = PSX_PadData(); // Null it out
                    pad                    = IInputDevice();
                    if (port == 0)
                        psx_sio0.setMultitapState(_portNumber, MT_TEST_PRESENCE);
                }
                break;
            default: break; // If SIO0 is in use (somehow on a single threaded app..) ignore.
            }

            if (!psx_sio0.multitapPresent(_portNumber)) break; // No need to scan further
            port++;
        }

        return (fr == GV_OK ? 0 : 1);
    }

    int PSX_Joypad::init()
    {
        LOG("psxpad", "Init PlayStation Controller driver on port %d", (_portNumber == SIO_CTRL_CS_PORT_1 ? 1 : 2));
        return psx_sio0.init();
    }

    bool PSX_Joypad::reset()
    {
        for (auto &pad : _padList)
        {
            Services::dettachInputDevice(&pad);
            pad = IInputDevice(); // Null device
        }
        memset(_padData, 0, sizeof(PSX_PadData));
        return true;
    }

    // Enter or exit controller config mode. Only works on DualShock and above
    int PSX_Joypad::configMode_(bool state, uint8_t subport)
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

        return SIO0_OKAY;
    }

    // Enables setting analog button and locking it. Only works on DualShock and above
    int PSX_Joypad::setAnalog_(bool state, bool lock, uint8_t subport)
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

        return SIO0_OKAY;
    }

    // Enables dual motors. Only works on DualShock and above
    int PSX_Joypad::setDualshock_(bool state, uint8_t subport)
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

        return SIO0_OKAY;
    }

    // Enables reading analog buttons. Only works on DualShock2
    // Configures how the normal controller response sequence works
    int PSX_Joypad::setDS2Analog_(uint32_t bitmask, uint8_t subport)
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
    }

    // Polls a standard controller on given subport (of multitap)
    int PSX_Joypad::poll(ControllerReadResponse &resp, uint8_t subport)
    {
        // Send a poll command. Also send a multitap enable command. If MT is not present, controller will respond
        // and we detect this (or MT will respond with ID) and then we turn off this test. When we get NO_ACK from
        // the SIO0 driver, start testing again. Theory is that after NO_ACKs, controller was unplugged and that
        // whilst we still have a controller plugged in, if it responded as a multitap, then we can assume it's always one.
        // We use the second method of polling when using a multitap so it's more flexible, just using method 1 to test presence.

        MultitapState _mt = psx_sio0.getMultitapState(_portNumber);

        const uint8_t request[4]{CMD_POLL, (uint8_t)(_mt == MT_TEST_PRESENCE ? 1 : 0), 0, 0};
        alignas(ControllerReadResponse) uint8_t response[(2 + ((2 * 4) * 4))]; // 2 ID bits, 4 'half-words (uint16_t)' of controller data, 4 contollers

        START(ADDR_CONTROLLER + subport, _portNumber);
        size_t respLength = psx_sio0.exchangeBytes(
            request,
            response,
            sizeof(request),
            sizeof(response));
        END(response, respLength);

        resp = ControllerReadResponse(response, respLength);
        if (_mt == MT_TEST_PRESENCE)
        {
            if (resp.idLo == PAD_MULTITAP)
            {
                psx_sio0.setMultitapState(_portNumber, MT_IS_PRESENT);
                resp = ControllerReadResponse(&response[2], respLength); // Port A data
            }
            else
            {
                psx_sio0.setMultitapState(_portNumber, MT_NOT_PRESENT);
            }
        }
        return SIO0_OKAY;
    }
} // namespace System::PSX::IO