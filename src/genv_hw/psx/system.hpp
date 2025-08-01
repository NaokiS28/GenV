/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * system.hpp - Created on 25-04-2025
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

#include <stdbool.h>

#include "common/services/services.hpp"
#include "common/services/sys/timer.hpp"

//#include "video/video.hpp"
//#include "audio/audio.hpp"
//#include "file/file.hpp"

namespace System
{
    namespace PSX {
        constexpr const char* szSystemName = "PlayStation";
        constexpr const char* szMakeName = "Sony";
    }

    /*
     * PSX System base class
     * This system implements the code neccesary to run GenV on a PlayStation 1.
     * Any system that is based on the PlayStation 1 can be derrived from this
     * class, where the Audio, Video, Input and File storage modules can be changed
     * out.
     * 
     * When making a derrived system, if the base function is NOT overriden, it will
     * be assumed that the derrived system uses the same functionality as the
     * PlayStation 1. An example of this is the System 573 derivative where the CPU
     * and GPU are the same, but the audio, input and files system change (audio is
     * expanded upon with the Digital Sound IO board)
    */
    class PSXSystem : public ISystem
    {
    private:
        uint8_t sm_state; // System Manager state for returning to main.cpp

        virtual int initVideo();
        virtual int initAudio();
        virtual int initIO() { return 0; }
        virtual int initFiles();

        inline bool linkServices()
        {
            Services::setSystem(this);
            return true;
        }

        SystemInfo siPSX = {
            .type = SYS_Console,
            .make = PSX::szMakeName,
            .name = PSX::szSystemName,
            .flags = SYS_No_Window_Mode
        };

    public:
        PSXSystem() : sm_state(System::SM_NORMAL)
        {
        }
        virtual ~PSXSystem() = default;

        virtual bool init() override;             // Registers Windows app class and inits drivers
        virtual int update() override;            // Process wWindows messages
        virtual bool shutdown() override;         // Prepare drivers and app for close
        virtual bool setResolution(int w, int h); // Sets window resolution (internal viewport)
        bool setFullscreen(Video::FullscreenMode mode){ return false; }
        bool toggleFullscreen(){ return false; }

        virtual const SystemInfo* getSysInfo() const override {
            return &siPSX;
        }

        size_t millis();

        inline bool registerTimerFunc(TFunc func, TChannel timer, uint8_t freq)
        {
            //if (timer == TChannel::TIMER1)
                //return sysTimer.registerFunction(func, freq);
            return false;
        }
        inline bool unregisterTimerFunc(TFunc func, TChannel timer)
        {
            //if (timer == TChannel::TIMER1)
                //return sysTimer.unregisterFunction(func);
            return false;
        }
    };

}
