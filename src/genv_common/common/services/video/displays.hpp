/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * displays.hpp - Created on 18-07-2026
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

#include <stddef.h>

#include "common/util/hash.hpp"

namespace Video
{
    // Slot-derived default screen identities. Screen slot i carries the default
    // name "DISPLAY{i+1}" - 0-based slot index, 1-based human name.
    //
    // When a screen registers without an explicit name, the allocator stamps
    // GV_DISPLAY(slot); an explicit name ("MARQUEE"_h) given at register time
    // overrides the slot default.
    constexpr size_t kMaxDisplays = 8;

    constexpr const char *const kDisplayName[kMaxDisplays] = {
        "DISPLAY1", "DISPLAY2", "DISPLAY3", "DISPLAY4",
        "DISPLAY5", "DISPLAY6", "DISPLAY7", "DISPLAY8"};

    constexpr util::Hash kDisplayHash[kMaxDisplays] = {
        "DISPLAY1"_h, "DISPLAY2"_h, "DISPLAY3"_h, "DISPLAY4"_h,
        "DISPLAY5"_h, "DISPLAY6"_h, "DISPLAY7"_h, "DISPLAY8"_h};

    // 0-based slot accessors. GV_DISPLAY(0) == hash of "DISPLAY1".
    constexpr util::Hash GV_DISPLAY(size_t slot)
    { return kDisplayHash[slot]; }
    constexpr const char *GV_DISPLAY_NAME(size_t slot)
    { return kDisplayName[slot]; }
} // namespace Video
