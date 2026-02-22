/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * iostrings.hpp - Created on 22-02-2026
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

#include "common/util/hash.hpp"

namespace IO
{
    constexpr util::Hash idNull       = 0;
    const char szNullInputDevice[]    = "Null input device";
    const char szGenericInputDevice[] = "Generic input device";

    constexpr const char szInputFailedFmt[] = "%s %s failed to %s: %s";
    constexpr const char szInputErrorFmt[]  = "%s %s gave error whilst %s: %08X";
    constexpr const char szInputFmt[]       = "%s %s %sed.";
    constexpr const char szAttach[]         = "attach";
    constexpr const char szDetach[]         = "detach";
    constexpr const char szInit[]           = "initialising";
    constexpr const char szRegister[]       = "register";
    constexpr const char szUnregister[]     = "unregister";
    constexpr const char szInput[]          = "Input";
    constexpr const char szDriver[]         = "driver";
    constexpr const char szDevice[]         = "device";

    constexpr const char szDeviceNull[] = "device pointer is null.";
} // namespace IO