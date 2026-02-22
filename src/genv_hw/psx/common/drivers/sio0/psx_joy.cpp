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

#include <stdint.h>
#include <string.h>

#include "common/logger/log.hpp"
#include "common/return_codes.hpp"
#include "common/services/io/iface_input.hpp"
#include "common/services/io/vjoy.hpp"
#include "common/services/services.hpp"

#include "psx/common/drivers/sio0/psx_pads.hpp"
#include "psx/common/drivers/sio0/psx_sio0.hpp"
#include "psx/common/psx_strings.hpp"

#define LOG_JOY(fmt, ...) LOG("psx_joy", fmt __VA_OPT__(, ) __VA_ARGS__)

#define BOOL(val) ((val) > 0 ? 1 : 0)

namespace System::PSX::IO
{
    uint8_t PSX_Joypad::m_driverCount = 0;

    // All controllers seem to use the same "base" map
    constexpr VJoy_Input controllerButtonMap[] = {
        VJoy_Input::Menu,      // Select
        VJoy_Input::Button_9,  // L3
        VJoy_Input::Button_10, // R3
        VJoy_Input::Start,     // Start
        VJoy_Input::D_Up,      // D-Pad Up
        VJoy_Input::D_Right,   // D-Pad Right
        VJoy_Input::D_Down,    // D-Pad Down
        VJoy_Input::D_Left,    // D-Pad Left
        VJoy_Input::Button_5,  // L1
        VJoy_Input::Button_6,  // R1
        VJoy_Input::Button_7,  // L2
        VJoy_Input::Button_8,  // R2
        VJoy_Input::Button_4,  // ∆
        VJoy_Input::Button_2,  // O
        VJoy_Input::Button_1,  // X
        VJoy_Input::Button_3,  // □
    };

    // TODO: Add support for PSX mouse in VMouse.
    // constexpr VJoy_Input mouseButtonMap[] = {};

    IInputDevice makePad(
        const char *name, util::Hash type, Multitap_Port subport,
        uint32_t *digital, uint8_t numDigital = 14,
        int16_t *analog = nullptr, uint8_t numAnalog = 0)
    {
        return IInputDevice(
            name,
            PSX_PAD_HASH,
            type,
            Input::DEVICE_TYPE_CONTROLLER,
            Input::DEVICE_SUBTYPE_STANDARD,
            static_cast<uint8_t>(subport),
            numDigital, digital,
            numAnalog, analog);
    }

    IInputDevice makeGun(
        const char *name, util::Hash type, Multitap_Port subport,
        uint32_t *digital, int16_t *analog)
    {
        return IInputDevice(
            name,
            PSX_PAD_HASH,
            type,
            Input::DEVICE_TYPE_LIGHTGUN,
            Input::DEVICE_SUBTYPE_STANDARD,
            static_cast<uint8_t>(subport),
            3, digital,
            2, analog);
    }

    IInputDevice makeMouse(Multitap_Port subport, uint32_t *digital, int16_t *delta)
    {
        return IInputDevice(
            PSX_MOUSE_STR,
            PSX_PAD_HASH,
            PSX_MOUSE_HASH,
            Input::DEVICE_TYPE_MOUSE,
            Input::DEVICE_SUBTYPE_STANDARD,
            static_cast<uint8_t>(subport),
            3, digital,
            0, nullptr,
            2, delta);
    }

    IInputDevice addController(
        const ControllerReadResponse &resp,
        Multitap_Port subport,
        uint32_t *digital,
        int16_t *analog = nullptr)
    {
        switch (resp.id8[0])
        {
        case PAD_DIGITAL: return makePad(PSX_DIGITAL_STR, PSX_DIGITAL_HASH, subport, digital, 14);
        case PAD_ANALOG: return makePad(PSX_ANALOG_STR, PSX_ANALOG_HASH, subport, digital, 16, analog, 4);
        case PAD_DUALSHOCK2: return makePad(PSX_DUALSHOCK2_STR, PSX_DUALSHOCK2_HASH, subport, digital, 16, analog, 10);
        case PAD_TWINSTICK: return makePad(PSX_TWINSTICK_STR, PSX_TWINSTICK_HASH, subport, digital, 14, analog, 4);
        case PAD_GUNCON: return makeGun(PSX_GUNCON_STR, PSX_GUNCON_HASH, subport, digital, analog);
        case PAD_KONAMI_GUN: return makeGun(PSX_JUSTIFIER_STR, PSX_JUSTIFIER_HASH, subport, digital, analog);
        case PAD_MOUSE: return makeMouse(subport, digital, analog);
        // case PAD_KEYBOARD: break;
        case PAD_NEGCON: return makePad(PSX_NEGCON_STR, PSX_NEGCON_HASH, subport, digital, 7, analog, 4);
        // case PAD_JOGCON: return psxJogcon(player, subport, digital, delta);
        default: return IInputDevice();
        }
    }

    void PSX_Joypad::m_padChange(PSX_PadData &pad, const ControllerReadResponse &resp, const Multitap_Port subport)
    {
        // Device changed
        LOG("makepad", "Controller changed on port %d:%c to 0x%04X (%s)",
            sioPortNumber(m_portNumber),
            multitapPortLetter(subport),
            resp.id,
            getPadName(resp.id));

        /*
            if (m_pads[subport].doDSTest)
        {
            m_pads[subport].doDSTest = false;
            // Config mode only works on DualShock and above
            if (!m_configMode(true, subport))
            {
                m_setAnalog(true, false, subport);
                psx_delayMicrosecondsBusy(6000);
                m_setDualshock(true, subport);
                psx_delayMicrosecondsBusy(6000);

                // setDS2Analog_(0x3FFFF, subport);
                m_configMode(false, subport);
                psx_delayMicrosecondsBusy(6000);

                poll(resp, subport);

                LOG("makepad", "Set controller mode on port %d:%c to 0x%04X (%s)",
                    sioPortNumber(m_portNumber),
                    multitapPortLetter(subport),
                    resp.id,
                    getPadName(resp.id));
            }
        }*/

        pad.type = static_cast<JoypadType>(resp.id8[0]);
        if (pad.device.type != Input::DEVICE_TYPE_NULL)
            getServiceManager()->dettachInputDevice(&pad.device);

        pad.device = addController(
            resp, subport,
            &pad.digital,
            pad.analog);

        if (pad.device.type != Input::DEVICE_TYPE_NULL)
            getServiceManager()->attachInputDevice(
                &pad.device, m_bus->psxPlayerSelect(m_portNumber, subport));

        pad.digital = 0;
        memset(pad.analog, (int)0, sizeof(m_pads->analog));
        memset(pad.rotary, (int)0, sizeof(m_pads->rotary));
    }

    void PSX_Joypad::m_padDisconnect(PSX_PadData &pad, const ControllerReadResponse &resp, const Multitap_Port subport)
    {
        // If Controller 1-A dissapears, the multi-tap stops responding.
        if (subport == Multitap_Port::PORTA && m_bus->multitapPresent(m_portNumber))
        {
            LOG("makepad", "Multitap disconnected on port %d", sioPortNumber(m_portNumber));
            for (auto &thisPad : m_pads)
            {
                if (thisPad.device.type != DEVICE_TYPE_NULL)
                {
                    LOG("makepad", "Controller disconnected on port %d:%c",
                        sioPortNumber(m_portNumber),
                        multitapPortLetter(subport));

                    getServiceManager()->dettachInputDevice(&thisPad.device);
                    thisPad = PSX_PadData(); // Null it out
                }
            }
            m_bus->setMultitapState(m_portNumber, MT_TEST_PRESENCE);
        }
        else
        {
            LOG("makepad", "Controller disconnected on port %d:%c",
                sioPortNumber(m_portNumber),
                multitapPortLetter(subport));

            if (pad.device.type != Input::DEVICE_TYPE_NULL)
                getServiceManager()->dettachInputDevice(&pad.device);

            pad = PSX_PadData(); // Null it out
            if (subport == Multitap_Port::PORTA) m_bus->setMultitapState(m_portNumber, MT_TEST_PRESENCE);
        }
    }

    bool PSX_Joypad::update()
    {
        int result = GV_OK;

        auto subport = Multitap_Port::PORTA;

        // Will always do the first subport (assuming multitap is connected, else just first port)
        // TODO: Trim pad polling code down and use per-frame pacing
        // TODO: If controller in multitap with controller 1:B is connected whilst Port 2:A also connected to a controller prior, both are assigned player 2. Does not happen in reverse
        for (auto &pad : m_pads)
        {
            ControllerReadResponse resp;
            switch (poll(resp, static_cast<Multitap_Port>(subport)))
            {
            case SIO0_OKAY:
            {
                if (pad.type == PAD_DISCONNECTED || pad.type != resp.id8[0])
                    m_padChange(pad, resp, subport);

                pad.digital = 0;

                // Convert poll digital inputs to GenV map
                // if (pad.type != PAD_MOUSE)
                //{
                uint16_t x = (~resp.input) & 0xFFFF; // Inversion promotes to int
                while (x != 0)
                {
                    int lz  = __builtin_clz((unsigned int)x) - 16; // adjust for uint16_t
                    int bit = 15 - lz;                             // MSB index

                    pad.digital |= asUint32(controllerButtonMap[bit]);

                    x &= ~(1u << bit); // clear processed bit
                }
                //}

                if (pad.type == PAD_ANALOG || pad.type == PAD_TWINSTICK)
                {
                    pad.analog[0] = resp.left.x - 128;
                    pad.analog[1] = resp.left.y - 128;
                    pad.analog[2] = resp.right.x - 128;
                    pad.analog[3] = resp.right.y - 128;
                }

                // TODO: Mouse "button" bits 8&9 are signs for rotary?
                // TODO: Mouse rotary inputs aren't working. Either it's here or in vpad.
                if (pad.type == PAD_MOUSE)
                {
                    pad.rotary[0] = resp.right.x;
                    pad.rotary[1] = resp.right.y;
                }

                // TODO: JogCon will not enable the dial functions and stays in compatability mode until you enable it with a config command, even if you press the mode button. GenV doesn't do this yet.
                if (pad.type == PAD_JOGCON)
                {
                    // pad.rotary[0] = resp.right.val16;
                }
            }
            break;
            case SIO0_NO_RESPONSE:
                if (pad.type != PAD_DISCONNECTED)
                    m_padDisconnect(pad, resp, subport);
                break;
            default: break; // If SIO0 is in use (somehow on a single threaded app..) ignore.
            }

            if (!m_bus->multitapPresent(m_portNumber)) break; // No need to scan further
            subport++;
        }

        return result;
    }

    int PSX_Joypad::init()
    {
        LOG("makepad", "Init PlayStation Controller driver on port %d", sioPortNumber(m_portNumber));
        return m_bus->init();
    }

    bool PSX_Joypad::reset()
    {
        for (auto &pad : m_pads)
        {
            getServiceManager()->dettachInputDevice(&pad.device);
            pad = PSX_PadData(); // Null device
        }
        return true;
    }

    // Enter or exit controller config mode. Only works on DualShock and above
    int PSX_Joypad::m_configMode(bool state, uint8_t subport)
    {
        uint8_t request[4]{CMD_CONFIG, 0, state, 0};
        uint8_t response[8];

        BUS_START(m_bus, ADDR_CONTROLLER + subport, m_portNumber);
        size_t respLength = m_bus->exchangeBytes(
            request,
            response,
            sizeof(request),
            sizeof(response));
        BUS_END(m_bus, response, respLength, PREFIX_CONTROLLER);
        // LOG("makepad", "Config mode port: %d, %02X %02X%02X", (_portNumber == SIO0_Port::PORT1 ? 1 : 2), state, response[1], response[0]);
        return SIO0_OKAY;
    }

    // Enables setting analog button and locking it. Only works on DualShock and above
    int PSX_Joypad::m_setAnalog(bool state, bool lock, uint8_t subport)
    {
        const uint8_t request[4]{CMD_SET_ANALOG, 0, state, (uint8_t)(lock ? 0x3 : 0x0)};
        uint8_t response[8];

        BUS_START(m_bus, ADDR_CONTROLLER + subport, m_portNumber);
        size_t respLength = m_bus->exchangeBytes(
            request,
            response,
            sizeof(request),
            sizeof(response));
        BUS_END(m_bus, response, respLength, PREFIX_CONTROLLER);
        // LOG("makepad", "Set analog mode port: %d, %02X %02X%02X", (_portNumber == SIO0_Port::PORT1 ? 1 : 2), state, response[1], response[0]);
        return SIO0_OKAY;
    }

    // Enables dual motors. Only works on DualShock and above
    int PSX_Joypad::m_setDualshock(bool state, uint8_t subport)
    {
        const uint8_t request[]{CMD_REQ_CONFIG, 0, 0, state, 0xFF, 0xFF, 0xFF, 0xFF};
        uint8_t response[8];

        BUS_START(m_bus, ADDR_CONTROLLER + subport, m_portNumber);
        size_t respLength = m_bus->exchangeBytes(
            request,
            response,
            sizeof(request),
            sizeof(response));
        BUS_END(m_bus, response, respLength, PREFIX_CONTROLLER);
        // LOG("makepad", "Set dualshock mode port: %d, %02X %02X%02X", (_portNumber == SIO0_Port::PORT1 ? 1 : 2), state, response[1], response[0]);
        return SIO0_OKAY;
    }

    // Enables reading analog buttons. Only works on DualShock2
    // Configures how the normal controller response sequence works
    int PSX_Joypad::m_setDS2Analog(uint32_t bitmask, uint8_t subport)
    {
        uint8_t mask[] = {
            (uint8_t)(bitmask & 0xFF),
            (uint8_t)((bitmask >> 8) & 0xFF),
            (uint8_t)((bitmask >> 16) & 0xFF),
            (uint8_t)((bitmask >> 24) & 0xFF),
            0};
        const uint8_t request[]{CMD_RESP_CONFIG, 0, mask[0], mask[1], mask[2], mask[3], mask[4]};
        uint8_t response[8];

        BUS_START(m_bus, ADDR_CONTROLLER + subport, m_portNumber);
        size_t respLength = m_bus->exchangeBytes(
            request,
            response,
            sizeof(request),
            sizeof(response));
        BUS_END(m_bus, response, respLength, PREFIX_CONTROLLER);
        LOG("makepad", "Set dualshock 2 mode port: %08X, %02X %02X%02X", sioPortNumber(m_portNumber), bitmask, response[1], response[0]);
        return GV_OK;
    }

    // Polls a standard controller on given subport (of multitap)
    int PSX_Joypad::poll(ControllerReadResponse &resp, Multitap_Port subport)
    {
        // Send a poll command. Also send a multitap enable command. If MT is not present, controller will respond
        // and we detect this (or MT will respond with ID) and then we turn off this test. When we get NO_ACK from
        // the SIO0 driver, start testing again. Theory is that after NO_ACKs, controller was unplugged and that
        // whilst we still have a controller plugged in, if it responded as a multitap, then we can assume it's always one.
        // We use the second method of polling when using a multitap so it's more flexible, just using method 1 to test presence.

        // UPDATE: The multitap is a pain in the ass.
        // The rules it follows are as such:
        // * Mutlitap is not "enabled" if the 3rd command byte is not 0x01.
        // * Multitap, once "enabled", will respond to poll commands with it's ID.
        // * Multitap expects that if using method 1, or the "long poll" method, you will only use that.
        // * The long poll will return all data from all pads connected pads, and pad the data for non-existant devices or data.
        // * You should use 1MHz mode for method 1 to not eat too much CPU cycle time.
        // * Using method 2, or direct access, the multitap will passively pass through the data.
        // * However if using method 2, the multitap will lock up if the PS1 mouse is inserted in port D.
        // * The multitap will not lock up if you use method 1.
        // * Multitap will switch between modes from the next poll onwards.
        // * Multitap maintains Method 1 state until two polling cycles with Method 2.
        // * On power up, multitap is in Method 2.

        MultitapState multi_state = MT_NOT_PRESENT;
        if (subport == Multitap_Port::PORTA)
            multi_state = m_bus->getMultitapState(m_portNumber);

        uint8_t test = (uint8_t)(multi_state == MT_TEST_PRESENCE ? 1 : 0);
        uint8_t request[4]{CMD_POLL, test, 0, 0};
        alignas(ControllerReadResponse) uint8_t response[(2 + ((2 * 4) * 4))] = {0}; // 2 ID bits, 4 'half-words (uint16_t)' of controller data, 4 contollers

        uint8_t pass      = 0;
        size_t respLength = 0;

        // Multitap jank - Issuing a request above wont always result in the multitap being ready
        // So if we're testing, send two commands. First a dummy poll with the test bit, then
        // the actual poll command without test bit to get the special ID.
        do
        {
            BUS_START(m_bus, ADDR_CONTROLLER + static_cast<uint8_t>(subport), m_portNumber);
            respLength = m_bus->exchangeBytes(
                request,
                response,
                sizeof(request),
                sizeof(response));
            BUS_END(m_bus, response, respLength, PREFIX_CONTROLLER);
            request[1] = 0x00;
            pass++;
        } while (pass < (2 * test));

        resp = ControllerReadResponse(response, respLength);
        if (multi_state == MT_TEST_PRESENCE)
        {
            if (resp.id8[0] == PAD_MULTITAP)
            {
                LOG("makepad", "Found PlayStation multitap on port %d", sioPortNumber(m_portNumber));
                m_bus->setMultitapState(m_portNumber, MT_IS_PRESENT);
                resp = ControllerReadResponse(&response[2], respLength); // Port A data
            }
            else
            {
                LOG("makepad", "Did not find PlayStation multitap on port %d: 0x%04X", sioPortNumber(m_portNumber), resp.id);
                m_bus->setMultitapState(m_portNumber, MT_NOT_PRESENT);
            }
        }
        return SIO0_OKAY;
    }

    void PSX_Joypad::m_processPackets()
    {
    }

} // namespace System::PSX::IO