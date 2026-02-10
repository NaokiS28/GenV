/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * vjoy.hpp - Created on 07-02-2026
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

enum class VJoy_Input : uint32_t
{
    None         = 0,
    Start        = (1u << 0),
    Menu         = (1u << 1),
    Home         = (1u << 2),
    D_Up         = (1u << 3),
    D_Down       = (1u << 4),
    D_Left       = (1u << 5),
    D_Right      = (1u << 6),
    Left_D_Up    = D_Up,       // Left Direction Pad
    Left_D_Down  = D_Down,     // Left Direction Pad
    Left_D_Left  = D_Left,     // Left Direction Pad
    Left_D_Right = D_Right,    // Left Direction Pad
    Right_D_Up    = (1u << 7),  // Right Direction Pad
    Right_D_Down  = (1u << 8),  // Right Direction Pad
    Right_D_Left  = (1u << 9),  // Right Direction Pad
    Right_D_Right = (1u << 10), // Right Direction Pad
    Button_1  = (1u << 11),
    Button_2  = (1u << 12),
    Button_3  = (1u << 13),
    Button_4  = (1u << 14),
    Button_5  = (1u << 15),
    Button_6  = (1u << 16),
    Button_7  = (1u << 17),
    Button_8  = (1u << 18),
    Button_9  = (1u << 19),
    Button_10 = (1u << 20),
    Button_11 = (1u << 21),
    Button_12 = (1u << 22),
    Button_13 = (1u << 23),
    Button_14 = (1u << 24),
    Button_15 = (1u << 25),
    Button_16 = (1u << 26),
    Button_17 = (1u << 27),
    Button_18 = (1u << 28),
    Button_19 = (1u << 29),
    Button_20 = (1u << 30),
    Button_21 = (1u << 31),
};

inline constexpr VJoy_Input operator|(VJoy_Input a, VJoy_Input b)
{
    return static_cast<VJoy_Input>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline constexpr VJoy_Input operator&(VJoy_Input a, VJoy_Input b)
{
    return static_cast<VJoy_Input>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline constexpr VJoy_Input operator^(VJoy_Input a, VJoy_Input b)
{
    return static_cast<VJoy_Input>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
}

inline constexpr VJoy_Input &operator|=(VJoy_Input &a, VJoy_Input b)
{
    return a = a | b;
}

inline constexpr VJoy_Input &operator&=(VJoy_Input &a, VJoy_Input b)
{
    return a = a & b;
}

inline constexpr VJoy_Input &operator^=(VJoy_Input &a, VJoy_Input b)
{
    return a = a ^ b;
}

inline constexpr VJoy_Input operator~(VJoy_Input a)
{
    return static_cast<VJoy_Input>(~static_cast<uint32_t>(a));
}

inline constexpr bool operator!(VJoy_Input a)
{
    return static_cast<uint32_t>(a) == 0;
}

inline constexpr uint32_t to_uint32(VJoy_Input a)
{
    return static_cast<uint32_t>(a);
}

#define INPUT_NONE_1 VJoy_Input::None
#define INPUT_NONE_2 INPUT_NONE_1, INPUT_NONE_1
#define INPUT_NONE_3 INPUT_NONE_2, INPUT_NONE_1
#define INPUT_NONE_4 INPUT_NONE_3, INPUT_NONE_1
#define INPUT_NONE_5 INPUT_NONE_4, INPUT_NONE_1
#define INPUT_NONE_6 INPUT_NONE_5, INPUT_NONE_1
#define INPUT_NONE_7 INPUT_NONE_5, INPUT_NONE_2
#define INPUT_NONE_8 INPUT_NONE_5, INPUT_NONE_3
#define INPUT_NONE_9 INPUT_NONE_5, INPUT_NONE_4
#define INPUT_NONE_10 INPUT_NONE_5, INPUT_NONE_5
#define INPUT_NONE(x) INPUT_NONE_##x
