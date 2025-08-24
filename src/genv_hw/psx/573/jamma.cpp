/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * jamma.cpp - Created on 23-08-2025
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

#include "jamma.hpp"

namespace System::PSX
{
    uint32_t Sys573Jamma::getJAMMAInputs(void)
    {
        uint32_t inputs;

        inputs = SYS573_JAMMA_MAIN;
        inputs |= (SYS573_JAMMA_EXT1 & 0x0f00) << 8;
        inputs |= (SYS573_JAMMA_EXT2 & 0x0f00) << 12;
        inputs |= (SYS573_MISC_IN & 0x1f00) << 16;

        return inputs ^ 0x1fffffff;
    }
} // namespace System::PSX
