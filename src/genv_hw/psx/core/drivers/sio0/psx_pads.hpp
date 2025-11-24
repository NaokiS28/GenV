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

#include "psx/core/psx_strings.hpp"

#include "common/services/io/iface_input.hpp"
#include "common/util/hash.hpp"

namespace System::PSX::IO
{
    using namespace Input;

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

    constexpr IInputDevice psxPad(
        const char *name, util::Hash type, PlayerSuggestion player, uint8_t subport,
        uint32_t *digital, uint8_t numDig = 14,
        int16_t *analog = nullptr, uint8_t numAnalog = 0)
    {
        return {
            name,
            PSX_PAD_HASH,
            type,
            Input::DEVICE_TYPE_CONTROLLER,
            Input::DEVICE_SUBTYPE_STANDARD,
            subport,
            player,
            {numDig, numAnalog},
            {digital, analog, nullptr}};
    }

    constexpr IInputDevice psxGun(
        const char *name, util::Hash type, PlayerSuggestion player, uint8_t subport,
        uint32_t *digital, int16_t *analog)
    {
        return {
            name,
            PSX_PAD_HASH,
            type,
            Input::DEVICE_TYPE_LIGHTGUN,
            Input::DEVICE_SUBTYPE_STANDARD,
            subport,
            player,
            {3, 2}, // Normally I'd say not to hard code the digital button count but conveniently the only two PSX guns are 3.
            {digital, analog, nullptr}};
    }

    constexpr IInputDevice devMouse(PlayerSuggestion player, uint8_t subport, uint32_t *digital, int16_t *delta)
    {
        return {
            PSX_MOUSE_STR,
            PSX_PAD_HASH,
            PSX_MOUSE_HASH,
            Input::DEVICE_TYPE_MOUSE,
            Input::DEVICE_SUBTYPE_STANDARD,
            subport,
            player,
            {2, 0, 2},
            {digital, nullptr, delta}};
    }

} // namespace System::PSX::IO