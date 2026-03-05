/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * ide.cpp - Created on 05-03-2026
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

#include "ide.hpp"
#include "psx/common/system/sys.h"
#include "psx/sys573/io/asic.hpp"

namespace System573::IO::IDE
{
    constexpr int IDE_RESET_TIME = 100;

    void SetReset(bool state)
    {
        ASIC::Regs::IdeReset = !state;
    }

    void PulseReset(bool state)
    {
        SetReset(true);
        psx_delayMicrosecondsBusy(IDE_RESET_TIME);
        SetReset(false);
    }
} // namespace System573::IO::IDE