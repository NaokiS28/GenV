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

namespace PS1::IO
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

    constexpr const util::Hash PS1_PAD_HASH        = "PS1PAD"_h;
    constexpr const util::Hash PS1_DIGITAL_HASH    = "PS1DIGITAL"_h;
    constexpr const util::Hash PS1_ANALOG_HASH     = "PS1ANALOG"_h;
    constexpr const util::Hash PS1_DUALSHOCK_HASH  = "PS1DUALSHOCK"_h;
    constexpr const util::Hash PS1_DUALSHOCK2_HASH = "PS1DUALSHOCK2"_h;
    constexpr const util::Hash PS1_GUNCON_HASH     = "PS1GUNCON"_h;
    constexpr const util::Hash PS1_TWINSTICK_HASH  = "PS1TWINSTICK"_h;
    constexpr const util::Hash PS1_JUSTIFIER_HASH  = "PS1JUSTIFIER"_h;
    constexpr const util::Hash PS1_MOUSE_HASH      = "PS1MOUSE"_h;
    constexpr const util::Hash PS1_KEYBOARD_HASH   = "PS1KEYBOARD"_h;
    constexpr const util::Hash PS1_NEGCON_HASH     = "PS1NEGCON"_h;
    constexpr const util::Hash PS1_JOGCON_HASH     = "PS1JOGCON"_h;

    constexpr const char *getPadName(uint16_t id)
    {
        switch (id & 0xFF)
        {
        case PAD_DIGITAL: return PS1_DIGITAL_STR;
        case PAD_ANALOG: return PS1_ANALOG_STR;
        case PAD_DUALSHOCK2: return PS1_DUALSHOCK2_STR;
        case PAD_TWINSTICK: return PS1_TWINSTICK_STR;
        case PAD_GUNCON: return PS1_GUNCON_STR;
        case PAD_KONAMI_GUN: return PS1_JUSTIFIER_STR;
        case PAD_MOUSE: return PS1_MOUSE_STR;
        // case PAD_KEYBOARD: return PS1_KEYBOARD_STR;
        case PAD_NEGCON: return PS1_NEGCON_STR;
        // case PAD_JOGCON: return PS1_JOGCON_STR;
        default: return "Unsupported controller";
        }
    }

} // namespace PS1::IO