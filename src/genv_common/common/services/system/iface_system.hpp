/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * iface_system.hpp - Created on 09-08-2025
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

#include <stdint.h>
#include <stdlib.h>

#include "timer.hpp"
#include "system.hpp"
#include "common/services/adminkey.hpp"
#include "common/services/video/iface_video.hpp"
#include "common/services/audio/iface_audio.hpp"
#include "common/services/storage/iface_storage.hpp"
#include "common/services/io/iface_input.hpp"

namespace System
{
    struct SystemInfo;

    class ISystem
    {
    protected:
        AdminClass_Key adminKey;

    public:
        ISystem() : adminKey(AdminClass_Key()) {};
        virtual ~ISystem() = default;
        ISystem(ISystem &other) = delete;
        void operator=(const ISystem &) = delete;

        virtual bool init() = 0;     // Init system then load and init hardware services
        virtual int update() = 0;    // Update system manager
        virtual bool shutdown() = 0; // Prepare for app shutdow

        virtual const SystemInfo *getSysInfo() const = 0;

        virtual size_t millis() = 0; // Millis since system start
        virtual size_t random(size_t min, size_t max)
        { // Gets a random number between given values
            return ((min + rand()) % max);
        }
        virtual size_t getTime() = 0;

        virtual bool registerTimerFunc(TFunc func, TChannel timer, uint8_t freq) = 0;
        virtual bool unregisterTimerFunc(TFunc func, TChannel timer) = 0;
    };
}