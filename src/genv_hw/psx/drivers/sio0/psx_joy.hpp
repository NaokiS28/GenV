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

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#include "psx_sio0.hpp"

#include "common/services/io/iface_input.hpp"
#include "psx/registers.hpp"

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
        FEEDBACK_NONE  = 0xFF,
        FEEDBACK_MOTOR = 0x01,
        FEEDBACK_DUALMOTOR,
        FEEDBACK_WHEEL,
    };

    enum ControllerType : uint8_t
    {
        TYPE_NONE         = 0,
        TYPE_MOUSE        = 1,
        TYPE_NEGCON       = 2,
        TYPE_IRQ10_GUN    = 3,
        TYPE_DIGITAL      = 4,
        TYPE_ANALOG_STICK = 5,
        TYPE_GUNCON       = 6,
        TYPE_ANALOG       = 7,
        TYPE_MULTITAP     = 8,
        TYPE_JOGCON       = 14,
        TYPE_CONFIG_MODE  = 15
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

    class PSX_Joypad : public IInput
    {
        friend class System::PSX::PSXSystem;
        friend class PortLock;

    private:
        static uint8_t driverCount;
        const SIOControlFlag _portNumber;

        struct
        {
            JoypadType type        = PAD_DISCONNECTED;
            uint8_t numAxis        = 0;
            uint8_t numDigital     = 0;
            FeedbackType motorType = FEEDBACK_NONE;
            uint8_t rumbleStrength = 0; // Rumble power strength (in 10% increments)
        } pad[4];

        int poll_(void);

        int configMode_(bool state);
        int setAnalog_(bool state = true, bool lock = true);
        int setDualshock_(bool state = true);
        int setDS2Analog_(uint32_t bitmask = 0x3FFFF);

    public:
        inline PSX_Joypad(uint8_t port) : _portNumber((port % 2) ? SIO_CTRL_CS_PORT_1 : SIO_CTRL_CS_PORT_2)
        {
            assert(driverCount < 2 && port <= 2);
        };

        inline int init() override
        {
            return psx_sio0.init_();
        }

        int update() override;
    };
} // namespace System::PSX::IO