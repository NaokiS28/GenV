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
#include "registers.h"

#include "video/video.hpp"
#include "system/sys.h"

#include "common/services/services.hpp"
#include "common/util/log.hpp"
#include "common/util/misc.hpp"


namespace System::PSX
{
    bool PSXSystem::init()
    {        
        IRQ_MASK = 0;
        IRQ_STAT = 0;
        DMA_DPCR = 0;
        DMA_DICR = DMA_DICR_CH_STAT_BITMASK;
        cop0_setSR(COP0_SR_Im2 | COP0_SR_CU0 | COP0_SR_CU2);

        if (initVideo() != 0)
        {
            return 1;
        }
        if (initFiles() != 0)
        {
            return 1;
        }
        if (initAudio() != 0)
        {
            return 1;
        }
        if (initIO() != 0)
        {
            return 1;
        }

        enableInterrupts();

        return true;
    }

    bool PSXSystem::setResolution(int w, int h)
    {
        // if (!video())
        return false;

        sm_state = SM_RESIZE;
        // return video()->setResolution(w, h);
    }

    int PSXSystem::initVideo()
    {
        Video::IVideo *vDriver = new GPU::PSXGPU;
        if (!vDriver || vDriver->init())
            return -1;
        Services::setVideo(adminKey, vDriver);
        IRQ_MASK = 1 << IRQ_VSYNC;
        return 0;
    }

    int PSXSystem::initAudio()
    {
        // IAudio *aDriver = Win32::CreateAudioDriver(Win32::AD_WIN_DSOUND, gpuWnd);
        // if (!aDriver || !aDriver->init())
        return -2;

        // Services::setVideo(adminKey, aDriver);
        // return 0;
    }

    int PSXSystem::initFiles()
    {
        return -2;

        // Services::setVideo(adminKey, &storage);
        // return 0;
    }

    int PSXSystem::update()
    {
        sm_state = SM_NORMAL;

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