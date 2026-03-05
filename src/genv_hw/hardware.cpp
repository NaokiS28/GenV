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
#include "common/services/system/arcade/iface_arcade.hpp"
#include "common/services/system/iface_system.hpp"

#if defined(GENV_PSX)
#if defined(GENV_SYS573)
#include "psx/sys573/system.hpp"
#include "common/services/services.hpp"
namespace System
{
    ISystem *makeNewSystem()
    {
        System573::Sys573System *system = new System573::Sys573System;
        return system;
    }
    IArcadeSystem *getArcadeInterface()
    {
        static ISystem *sys = nullptr;
        if (!sys) sys = getServiceManager()->getSystem();
        return reinterpret_cast<System573::Sys573System *>(sys);
    }
} // namespace System
#else
#include "psx/psx/system.hpp"
namespace System
{
    ISystem *makeNewSystem()
    {
        PSX::PSXSystem *system = new PSX::PSXSystem;
        return system;
    }
    IArcadeSystem *getArcadeInterface()
    {
        return nullptr;
    }
} // namespace System
#endif
#elif defined(GENV_WIN32)
// Windows Targets
#include "win32/targetver.h"
#include "win32/system.hpp"
namespace System
{
    ISystem *makeNewSystem()
    {
        WinSystem *system = new WinSystem();
        return system;
    }
    IArcadeSystem *getArcadeInterface()
    {
        return nullptr;
    }
} // namespace System
#elif defined(GENV_MACOSX)
#include "mac/osx/system.hpp"
namespace System
{
    ISystem *makeNewSystem()
    {
        OSXSystem *system = new OSXSystem();
        return system;
    }
    IArcadeSystem *getArcadeInterface()
    {
        return nullptr;
    }
} // namespace System
#elif defined(GENV_MACOS9)
#include "mac/osx/system.hpp"
namespace System
{
    ISystem *makeNewSystem()
    {
        OS9System *system = new OS9System();
        return system;
    }
    IArcadeSystem *getArcadeInterface()
    {
        return nullptr;
    }
} // namespace System
#else
#error "Either no system defined or system not supported. No ISystem class exists"
#endif