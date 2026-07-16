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
#include "common/return_codes.hpp"
#include "common/services/adminkey.hpp"
#include "common/services/system/device_mgr.hpp"
#include "common/services/system/iface_system.hpp"

namespace System
{
    // Public verbs
    ISystem *shared_sys_ptr = nullptr;

    //!Review
    // Route the free-function accessor through the active system. Returns nullptr when
    // there is no system yet or the index is unregistered (the old body returned nothing
    // -> undefined behaviour on every call).
    Screen *screen(uint8_t idx)
    {
        return shared_sys_ptr ? shared_sys_ptr->getScreen(idx) : nullptr;
    }
    //!End

    size_t millis()
    {
        ISystem *sys = getServiceManager()->getSystem();
        if (sys)
            return sys->millis();
        return false;
    }

    size_t micros()
    {
        ISystem *sys = getServiceManager()->getSystem();
        if (sys)
            return sys->micros();
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

    BaseSystem::BaseSystem(ServiceManager &services)
        : adminKey(AdminClass_Key()),
          services(services),
          deviceManager()
    {
        // Was `assert(shared_sys_ptr)` - inverted. The singleton pointer must be EMPTY
        // before this system claims it; asserting it is already set fires on the only
        // legitimate construction.
        assert(!shared_sys_ptr);
        shared_sys_ptr = this;
    }

    int BaseSystem::initCore()
    {
        deviceManager.init();
        s_screens.init();
        return GV_OK;
    }

    int BaseSystem::initVideo()
    {
        return GV_OK;
    }

    int BaseSystem::initAudio()
    {
        return GV_OK;
    }

    int BaseSystem::initIO()
    {
        return GV_OK;
    }

    int BaseSystem::update()
    {
        deviceManager.update();
        return GV_OK;
    }

    void BaseSystem::shutdown()
    {
        deviceManager.shutdown();
    }
} // namespace System