/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * system.hpp - Created on 07-05-2025
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
#include "video/video.hpp"
#include "sys/timer.hpp"

namespace System
{
    // System status messages for reporting conditions requiring the application's attention.
    enum : uint8_t
    {
        SM_NORMAL,
        SM_RELOAD,
        SM_RESIZE,
        SM_REPOSITION,
        SM_QUIT
    };

    /*
        System Interface
        ============
        This interface is the one the main program loop uses to handle the 'system', or
        the host device the code is running on. This is to abstract the platform specific
        code away from the game/application code. The main objectives of this class is to
        init the hardware/platform of the system and then manage platform specific drivers.

        The system manager interface should create and init the following drivers at the
        minimum during init():
        * Video
        * Audio
        * Input
        * File/Block storage devices

        Once created and init'd, the driver instances should be set to the global space
        using:
        * Services::setVideo(IVideo *gpu);
        * Services::setAudio(IAudio *audio);
        * Services::setInput(IInput *input);
    */

    class ISystem
    {
    public:
        ISystem() = default;
        virtual ~ISystem() = default;
        ISystem(ISystem &other) = delete;
        void operator=(const ISystem &) = delete;

        virtual bool init() = 0;     // Init system then load and init hardware services
        virtual int update() = 0;    // Update system manager
        virtual bool shutdown() = 0; // Prepare for app shutdow

        virtual size_t millis() = 0;                        // Millis since system start
        virtual size_t random(size_t min, size_t max){      // Gets a random number between given values
            return ((min + rand()) % max);
        }

        virtual bool registerTimerFunc(TFunc func, TChannel timer, uint8_t freq) = 0;
        virtual bool unregisterTimerFunc(TFunc func, TChannel timer) = 0;

    };
}