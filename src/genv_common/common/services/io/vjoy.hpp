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

#include "common/util/enum_defs.hpp"

enum class VJoy_Input : uint32_t
{
    None          = 0,
    Start         = (1u << 0),
    Menu          = (1u << 1),
    Home          = (1u << 2),
    D_Up          = (1u << 3),
    D_Down        = (1u << 4),
    D_Left        = (1u << 5),
    D_Right       = (1u << 6),
    Left_D_Up     = D_Up,       // Left Direction Pad
    Left_D_Down   = D_Down,     // Left Direction Pad
    Left_D_Left   = D_Left,     // Left Direction Pad
    Left_D_Right  = D_Right,    // Left Direction Pad
    Right_D_Up    = (1u << 7),  // Right Direction Pad
    Right_D_Down  = (1u << 8),  // Right Direction Pad
    Right_D_Left  = (1u << 9),  // Right Direction Pad
    Right_D_Right = (1u << 10), // Right Direction Pad
    Button_1      = (1u << 11),
    Button_2      = (1u << 12),
    Button_3      = (1u << 13),
    Button_4      = (1u << 14),
    Button_5      = (1u << 15),
    Button_6      = (1u << 16),
    Button_7      = (1u << 17),
    Button_8      = (1u << 18),
    Button_9      = (1u << 19),
    Button_10     = (1u << 20),
    Button_11     = (1u << 21),
    Button_12     = (1u << 22),
    Button_13     = (1u << 23),
    Button_14     = (1u << 24),
    Button_15     = (1u << 25),
    Button_16     = (1u << 26),
    Button_17     = (1u << 27),
    Button_18     = (1u << 28),
    Button_19     = (1u << 29),
    Button_20     = (1u << 30),
    Button_21     = (1u << 31),
};

ENABLE_BITWISE_OPS(VJoy_Input);

enum class VJoy_Arcade : uint32_t
{
    None         = 0,
    Test         = (1u << 0),
    Tilt         = (1u << 1),
    Coin_1       = (1u << 2),
    Coin_2       = (1u << 3),
    Coin_3       = (1u << 4),
    Coin_4       = (1u << 5),
    Coin_5       = (1u << 6),
    Coin_6       = (1u << 7),
    Coin_7       = (1u << 8),
    Coin_8       = (1u << 9),
    Service_1    = (1u << 10),
    Service_2    = (1u << 11),
    Service_3    = (1u << 12),
    Service_4    = (1u << 13),
    Service_5    = (1u << 14),
    Service_6    = (1u << 15),
    Service_7    = (1u << 16),
    Service_8    = (1u << 17),
    Interlock_1  = (1u << 18),
    Interlock_2  = (1u << 19),
    Interlock_3  = (1u << 20),
    Interlock_4  = (1u << 21),
    Interlock_5  = (1u << 22),
    Interlock_6  = (1u << 23),
    Interlock_7  = (1u << 24),
    Interlock_8  = (1u << 25),
    Interlock_9  = (1u << 26),
    Interlock_10 = (1u << 27),
    CoinMax      = Coin_8,
    ServiceMax   = Service_8,
    InterlockMax = Interlock_10
};

ENABLE_BITWISE_OPS(VJoy_Arcade);

inline constexpr VJoy_Arcade arcadeCoin(size_t coin)
{
    auto x = static_cast<uint32_t>(VJoy_Arcade::Coin_1);
    x <<= coin % 8; // 8 coins: Coin_1 through Coin_8
    return static_cast<VJoy_Arcade>(x);
}

inline constexpr VJoy_Arcade arcadeService(size_t service)
{
    auto x = static_cast<uint32_t>(VJoy_Arcade::Service_1);
    x <<= service % 8; // 8 services: Service_1 through Service_8
    return static_cast<VJoy_Arcade>(x);
}

inline constexpr VJoy_Arcade arcadeInterlock(size_t interlock)
{
    auto x = static_cast<uint32_t>(VJoy_Arcade::Interlock_1);
    x <<= interlock % 10; // 10 interlocks: Interlock_1 through Interlock_10
    return static_cast<VJoy_Arcade>(x);
}

// Takes active high inputs and returns GenV inputs from supplied table
template <typename T, size_t N>
uint32_t getInputsFromMap(T inputData, const VJoy_Input (&table)[N])
{
    static_assert(N >= (8 * sizeof(T)), "Table must cover all bits of input type");

    uint32_t result = 0;

    auto x = static_cast<uint32_t>(inputData); // assume already active-high

    while (x != 0)
    {
        int bit = __builtin_ctz(x); // index of lowest set bit
        result |= static_cast<uint32_t>(table[bit]);
        x &= x - 1; // clear lowest set bit
    }

    return result;
}

// Takes active high inputs and returns GenV inputs from supplied table
template <typename T, size_t N>
uint32_t getArcadeFromMap(T inputData, const VJoy_Arcade (&table)[N])
{
    static_assert(N >= (8 * sizeof(T)), "Table must cover all bits of input type");

    uint32_t result = 0;

    auto x = static_cast<uint32_t>(inputData); // assume already active-high

    while (x != 0)
    {
        int bit = __builtin_ctz(x); // index of lowest set bit
        result |= static_cast<uint32_t>(table[bit]);
        x &= x - 1; // clear lowest set bit
    }

    return result;
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

#define ARCADE_NONE_1 VJoy_Arcade::None
#define ARCADE_NONE_2 ARCADE_NONE_1, ARCADE_NONE_1
#define ARCADE_NONE_3 ARCADE_NONE_2, ARCADE_NONE_1
#define ARCADE_NONE_4 ARCADE_NONE_3, ARCADE_NONE_1
#define ARCADE_NONE_5 ARCADE_NONE_4, ARCADE_NONE_1
#define ARCADE_NONE_6 ARCADE_NONE_5, ARCADE_NONE_1
#define ARCADE_NONE_7 ARCADE_NONE_5, ARCADE_NONE_2
#define ARCADE_NONE_8 ARCADE_NONE_5, ARCADE_NONE_3
#define ARCADE_NONE_9 ARCADE_NONE_5, ARCADE_NONE_4
#define ARCADE_NONE_10 ARCADE_NONE_5, ARCADE_NONE_5
#define ARCADE_NONE(x) ARCADE_NONE_##x