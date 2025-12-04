/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * hardware.hpp - Created on 24-04-2025
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

// This file is used to include all hardware drives specific to the given platform.
#pragma once

#include "common/services/system/arcade/iface_arcade.hpp"
#include "common/services/system/iface_system.hpp"

namespace System
{
    // System factory
    ISystem *makeNewSystem();
    IArcadeSystem *getArcadeInterface();
} // namespace System