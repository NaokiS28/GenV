/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * psxjoy.hpp - Created on 23-08-2025
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

#include <cstdint>
#include <stddef.h>
#include <assert.h>
#include <string.h>

#include "common/services/io/iface_input.hpp"

#include "psx_sio0.hpp"
#include "psx/core/psx_strings.hpp"
#include "psx/core/registers.hpp"

namespace System::PSX
{
    class PSXSystem;
}

namespace System::PSX::IO
{
    using namespace ::Input;

    enum JoypadType : uint8_t
    {
        PAD_ERROR        = 0x00,
        PAD_DISCONNECTED = 0xFF,
        PAD_MOUSE        = 0x12,
        PAD_NEGCON       = 0x23,
        PAD_KONAMI_GUN   = 0x31,
        PAD_DIGITAL      = 0x41,
        PAD_TWINSTICK    = 0x53,
        PAD_GUNCON       = 0x63,
        PAD_ANALOG       = 0x73,
        PAD_DVD_REMOTE   = 0x12,
        PAD_DUALSHOCK2   = 0x79,
        PAD_MULTITAP     = 0x80,
        PAD_KEYBOARD     = 0x96,
        PAD_JOGCON       = 0xE3,
        PAD_CONFIG       = 0xF3,
    };

    enum FeedbackType : uint8_t
    {
        FEEDBACK_NONE = 0,
        FEEDBACK_MOTOR,
        FEEDBACK_DUALMOTOR,
        FEEDBACK_WHEEL,
    };

    enum ControllerButton : uint16_t
    {
        // Standard controllers
        BTN_SELECT   = 1 << 0,
        BTN_L3       = 1 << 1,
        BTN_R3       = 1 << 2,
        BTN_START    = 1 << 3,
        BTN_UP       = 1 << 4,
        BTN_RIGHT    = 1 << 5,
        BTN_DOWN     = 1 << 6,
        BTN_LEFT     = 1 << 7,
        BTN_L2       = 1 << 8,
        BTN_R2       = 1 << 9,
        BTN_L1       = 1 << 10,
        BTN_R1       = 1 << 11,
        BTN_TRIANGLE = 1 << 12,
        BTN_CIRCLE   = 1 << 13,
        BTN_CROSS    = 1 << 14,
        BTN_SQUARE   = 1 << 15,

        // Mouse
        BTN_MOUSE_RIGHT = 1 << 10,
        BTN_MOUSE_LEFT  = 1 << 11,

        // neGcon
        BTN_NEGCON_START = 1 << 3,
        BTN_NEGCON_UP    = 1 << 4,
        BTN_NEGCON_RIGHT = 1 << 5,
        BTN_NEGCON_DOWN  = 1 << 6,
        BTN_NEGCON_LEFT  = 1 << 7,
        BTN_NEGCON_R     = 1 << 11,
        BTN_NEGCON_B     = 1 << 12,
        BTN_NEGCON_A     = 1 << 13,

        // Guncon
        BTN_GUNCON_A       = 1 << 3,
        BTN_GUNCON_TRIGGER = 1 << 13,
        BTN_GUNCON_B       = 1 << 14,

        // IRQ10 lightgun
        BTN_IRQ10_GUN_START   = 1 << 3,
        BTN_IRQ10_GUN_BACK    = 1 << 14,
        BTN_IRQ10_GUN_TRIGGER = 1 << 15
    };

    struct ControllerReadResponse
    {
        union
        {
            uint8_t id8[2];
            uint16_t id = 0;
        };
        uint16_t input = 0;
        struct AnalogInput
        {
            uint8_t x = 0, y = 0;
        } left, right;

        ControllerReadResponse() {}
        ControllerReadResponse(const uint8_t *rsp, size_t len)
        {
            assert(len >= 4);
            id    = (uint16_t)((rsp[1] << 8) | rsp[0]);
            input = (uint16_t)((rsp[3] << 8) | rsp[2]);
            if (len > 4)
            {
                left.x  = rsp[4];
                left.y  = rsp[5];
                right.x = rsp[6];
                right.y = rsp[7];
            }
        }
    };

    class PSX_Joypad : public IInputDriver
    {
    private:
        static uint8_t driverCount;
        const SIOControlFlag _portNumber;

        struct PSX_PadData
        {
            uint32_t digital         = 0;
            int16_t analog[10]       = {0};
            uint8_t motorStrength[2] = {0};
            JoypadType type          = PAD_DISCONNECTED;
            bool doDSTest            = true;
        } _padData[4];

        IInputDevice _padList[4];

        int configMode_(bool state, uint8_t subport = 0);
        int setAnalog_(bool state = true, bool lock = true, uint8_t subport = 0);
        int setDualshock_(bool state = true, uint8_t subport = 0);
        int setDS2Analog_(uint32_t bitmask = 0x3FFFF, uint8_t subport = 0);

    public:
        inline PSX_Joypad(uint8_t port) : _portNumber((port % 2) ? SIO_CTRL_CS_PORT_1 : SIO_CTRL_CS_PORT_2)
        {
            assert(driverCount < 2 && port <= 2);
            _name = PSX_PS_CONTROLLER_STR;
        };

        int poll(ControllerReadResponse &resp, uint8_t subport = 0);

        int init() override;
        int update() override;
        bool reset() override;
        inline void shutdown() override { reset(); }
    };
} // namespace System::PSX::IO