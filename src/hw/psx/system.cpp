/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * system.cpp - Created on 25-04-2025
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
#include "resources.h"

#include "common/util/log.hpp"
#include "common/util/misc.hpp"

namespace System
{
    //IWinVideo *video(){ return static_cast<IWinVideo *>(Services::getVideo()); }
    //IAudio *audio(){ return Services::getAudio(); }

    bool PSXSystem::init()
    {
        // You must set the system service before initing further drivers
        linkServices();

        if (initVideo())
        {
            return 1;
        }
        if (initFiles())
        {
            return 1;
        }
        if (initAudio())
        {
            return 1;
        }
        if (initIO())
        {
            return 1;
        }

        //setResolution(0,0); // TODO: Fullscreen and Borderless are fucked up.
        return true;
    }

    bool PSXSystem::setResolution(int w, int h)
    {
        //if (!video())
            return false;

        sm_state = System::SM_RESIZE;
        //return video()->setResolution(w, h);
    }

    int PSXSystem::initVideo()
    {
        //Services::setVideo(vDriver);
        //video()->setResolution(gpuWnd->size.w, gpuWnd->size.h);
        return 0;
    }

    int PSXSystem::initAudio()
    {
        //IAudio *aDriver = Win32::CreateAudioDriver(Win32::AD_WIN_DSOUND, gpuWnd);
        //if (!aDriver || !aDriver->init())
            return -2;

        //Services::setAudio(aDriver);
        //return 0;
    }

    int PSXSystem::initFiles()
    {
        return -2;

        //Services::setStorage(&storage);
        //return 0;
    }

    int PSXSystem::update()
    {
        sm_state = System::SM_NORMAL;

        return sm_state;
    }

    bool PSXSystem::shutdown()
    {
        return true;
    }

    size_t PSXSystem::millis()
    {
        return 0;
    }
}