/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * hardware.cpp - Created on 09-08-2025
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

#include "hardware.hpp"

#if defined(GENV_PSX)
#include "psx/system.hpp"
    System::ISystem *System::makeNewSystem()
    {
        PSX::PSXSystem *system = new PSX::PSXSystem;
        return system;
    }

#elif defined(GENV_573)
#include "psx/573/system.hpp"
    System::ISystem *System::makeNewSystem()
    {
        PSX::573System *system = new PSX::573System();
        return system;
    }

#elif defined(GENV_WIN32)
// Windows Targets
#include "win32/targetver.h"
#include "win32/system.hpp"

    System::ISystem *System::makeNewSystem()
    {
        WinSystem *system = new WinSystem();
        return system;
    }

#else
#error "Either no system defined or system not supported. No ISystem class exists"
#endif
