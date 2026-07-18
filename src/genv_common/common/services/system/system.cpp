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
#include "common/services/video/displays.hpp"

namespace System
{
    // Public verbs
    ISystem *shared_sys_ptr = nullptr;

    // Route the free-function accessor through the active system. Returns nullptr when
    // there is no system yet or the index is unregistered (the old body returned nothing
    // -> undefined behaviour on every call).
    Screen *screen(uint8_t idx)
    {
        return shared_sys_ptr ? shared_sys_ptr->getScreen(idx) : nullptr;
    }

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

    //! Review
    // Allocate a screen for a video driver: take the next free slot, default its name
    // from the DISPLAY table when the driver didn't specify one.
    // Returns nullptr if the backing list can't grow.
    Video::Screen *BaseSystem::assignScreen(System::IVideoDriver *driver, const Video::ScreenConfig &cfg)
    {
        size_t slot      = s_screens.length();
        const char *name = cfg.name ? cfg.name
                                    : (slot < Video::kMaxDisplays ? Video::GV_DISPLAY_NAME(slot) : "DISPLAY");
        Video::Screen *s = new Video::Screen(
            driver, cfg.res, cfg.refresh, cfg.dpi, name, static_cast<uint8_t>(slot));
        // PointerList auto-expands; append only fails on allocation failure (OOM), at
        // which point the system is already dead, so no cleanup path is worthwhile.
        s_screens.append(s);
        return s;
    }
    //! End

    Video::Screen *BaseSystem::registerScreen(System::IVideoDriver *driver, Video::Screen *screen)
    {
        size_t slot = s_screens.length();
        if (!screen->getName())
        {
            auto name = (slot < Video::kMaxDisplays ? Video::GV_DISPLAY_NAME(slot) : "DISPLAY");
            screen->setName(adminKey, name);
        }
        // PointerList auto-expands; append only fails on allocation failure (OOM), at
        // which point the system is already dead, so no cleanup path is worthwhile.
        s_screens.append(screen);
        return screen;
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