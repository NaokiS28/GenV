/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * system.cpp - Created on 09-08-2025
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

#include "iface_system.hpp"
#include "../services.hpp"

namespace System
{
    IArcadeSystem *GetArcadeInterface()
    {
        System::ISystem *Genv_Sys = Services::getSystem();
        return (Genv_Sys && Genv_Sys->getSysInfo()->type == System::SYS_Arcade)
                   ? reinterpret_cast<IArcadeSystem *>(Genv_Sys)
                   : nullptr;
    };

    size_t millis(){ return Services::getSystem()->millis(); }
    size_t random(size_t min, size_t max){ return Services::getSystem()->random(min, max); }
    size_t getTime(){ return Services::getSystem()->getTime(); }
}