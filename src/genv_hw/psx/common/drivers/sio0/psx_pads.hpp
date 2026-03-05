/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * psx_pads.hpp - Created on 15-11-2025
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

#include "psx/common/psx_strings.hpp"

#include "common/util/hash.hpp"

namespace PSX::IO
{
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

    constexpr const util::Hash PSX_PAD_HASH        = "PSXPAD"_h;
    constexpr const util::Hash PSX_DIGITAL_HASH    = "PSXDIGITAL"_h;
    constexpr const util::Hash PSX_ANALOG_HASH     = "PSXANALOG"_h;
    constexpr const util::Hash PSX_DUALSHOCK_HASH  = "PSXDUALSHOCK"_h;
    constexpr const util::Hash PSX_DUALSHOCK2_HASH = "PSXDUALSHOCK2"_h;
    constexpr const util::Hash PSX_GUNCON_HASH     = "PSXGUNCON"_h;
    constexpr const util::Hash PSX_TWINSTICK_HASH  = "PSXTWINSTICK"_h;
    constexpr const util::Hash PSX_JUSTIFIER_HASH  = "PSXJUSTIFIER"_h;
    constexpr const util::Hash PSX_MOUSE_HASH      = "PSXMOUSE"_h;
    constexpr const util::Hash PSX_KEYBOARD_HASH   = "PSXKEYBOARD"_h;
    constexpr const util::Hash PSX_NEGCON_HASH     = "PSXNEGCON"_h;
    constexpr const util::Hash PSX_JOGCON_HASH     = "PSXJOGCON"_h;

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
        case PAD_MOUSE: return PSX_MOUSE_STR;
        // case PAD_KEYBOARD: return PSX_KEYBOARD_STR;
        case PAD_NEGCON: return PSX_NEGCON_STR;
        // case PAD_JOGCON: return PSX_JOGCON_STR;
        default: return "Unsupported controller";
        }
    }

} // namespace PSX::IO