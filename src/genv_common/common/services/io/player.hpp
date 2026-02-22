/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * player.hpp - Created on 22-02-2026
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
#include "common/util/enum_defs.hpp"

namespace IO
{
    // Player flags - Used for both player suggestions (device registration) and tracking available players.
    // --
    // Player::ANY:
    // If set to any, then this controller will be mapped to the next unmapped player (i.e. 1 >> 2 >> 3..8), else it will not be registered.
    // --
    // Player::n:
    // If the player number is set, it will tell the mapper to append this device to a specific player.
    // Example: If a device suggests only Player::PLAYER_2, then even if player 2 already has a device attached,
    // it will attach this second device and buttons on either will be used for player 2.
    // --
    // (Player::n | Player::n | ...):
    // If two or more player numbers are given, then the mapper will interpret this as saying to map to one of the given numbers,
    // with the lower player number giving priority if they dont have a controller attached.
    // If both players have a controller attached, then it will try to assign as a secondary controller to the lower player number first.
    // --
    // ARCADE_CABINET:
    // This is arcade specific!
    // This type is special and means this is the service panel controls.
    // Service controls MUST be given this suggestion to work, and multiple service controls can be appended.
    enum class Player : uint16_t
    {
        NONE           = 0,
        ANY            = 0,
        PLAYER_1       = (1 << 0),
        PLAYER_2       = (1 << 1),
        PLAYER_3       = (1 << 2),
        PLAYER_4       = (1 << 3),
        PLAYER_5       = (1 << 4),
        PLAYER_6       = (1 << 5),
        PLAYER_7       = (1 << 6),
        PLAYER_8       = (1 << 7),
        ARCADE_CABINET = (1 << 8),
        INVALID        = 0xFFFF
    };

    // Used with the device descriptor to tell PlayerManager which player the device is attached to (sequential indexing)
    enum class PlayerIndex : uint8_t
    {
        PLAYER_1       = 0,
        PLAYER_2       = 1,
        PLAYER_3       = 2,
        PLAYER_4       = 3,
        PLAYER_5       = 4,
        PLAYER_6       = 5,
        PLAYER_7       = 6,
        PLAYER_8       = 7,
        ARCADE_CABINET = 8,
        INVALID        = 0xFF
    };

    inline constexpr Player &operator++(Player &a)
    {
        if (a == Player::PLAYER_8)
            a = Player::PLAYER_1;
        else
            a = static_cast<Player>(static_cast<uint16_t>(a) << 1);
        return a;
    }

    inline constexpr Player &operator--(Player &a)
    {
        if (a == Player::PLAYER_1)
            a = Player::PLAYER_8;
        else
            a = static_cast<Player>(static_cast<uint16_t>(a) >> 1);
        return a;
    }

    inline constexpr Player operator++(Player &a, int)
    {
        if (a == Player::PLAYER_8)
            a = Player::PLAYER_1;
        else
            a = static_cast<Player>(static_cast<uint16_t>(a) << 1);
        return a;
    }

    inline constexpr Player operator--(Player &a, int)
    {
        if (a == Player::PLAYER_1)
            a = Player::PLAYER_8;
        else
            a = static_cast<Player>(static_cast<uint16_t>(a) >> 1);
        return a;
    }

    ENABLE_BITWISE_OPS(Player);

    // Helper function to convert player index (0-8) to Player flag
    inline constexpr Player playerIndexToFlag(uint8_t idx)
    {
        constexpr Player flags[] = {
            Player::PLAYER_1,
            Player::PLAYER_2,
            Player::PLAYER_3,
            Player::PLAYER_4,
            Player::PLAYER_5,
            Player::PLAYER_6,
            Player::PLAYER_7,
            Player::PLAYER_8,
            Player::ARCADE_CABINET};
        return (idx < 9) ? flags[idx] : Player::NONE;
    }

    inline constexpr PlayerIndex playerToIndex(Player player)
    {
        switch (player)
        {
        default: return PlayerIndex::INVALID;
        case Player::PLAYER_1: return PlayerIndex::PLAYER_1;
        case Player::PLAYER_2: return PlayerIndex::PLAYER_2;
        case Player::PLAYER_3: return PlayerIndex::PLAYER_3;
        case Player::PLAYER_4: return PlayerIndex::PLAYER_4;
        case Player::PLAYER_5: return PlayerIndex::PLAYER_5;
        case Player::PLAYER_6: return PlayerIndex::PLAYER_6;
        case Player::PLAYER_7: return PlayerIndex::PLAYER_7;
        case Player::PLAYER_8: return PlayerIndex::PLAYER_8;
        case Player::ARCADE_CABINET: return PlayerIndex::ARCADE_CABINET;
        }
    }
} // namespace IO