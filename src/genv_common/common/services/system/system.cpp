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

#include "system.hpp"
#include "../services.hpp"

namespace System
{
    size_t millis()
    {
        ISystem *sys = getServiceManager()->getSystem();
        if (sys)
            return sys->millis();
        return false;
    }

    size_t random(size_t min, size_t max)
    {
        ISystem *sys = getServiceManager()->getSystem();
        if (sys)
            return sys->random(min, max);
        return false;
    }

    bool getTime(tm &time)
    {
        ISystem *sys = getServiceManager()->getSystem();
        if (sys)
            return sys->getTime(time);
        return false;
    }

    size_t getTime()
    {
        return 0;
    }

    BaseSystem::BaseSystem() : services(*getServiceManager()), adminKey(AdminClass_Key()) {}

} // namespace System