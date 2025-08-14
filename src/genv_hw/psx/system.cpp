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
#include "common/logger/log.hpp"
#include "common/util/misc.hpp"
#include "system/serial.h"
#include "terminal/terminal.h"

namespace System::PSX
{
    PSXSystem::PSXSystem() : sm_state(SM_NORMAL)
    {
        GenV_ConsoleOps ops;
        ops.init = &sio1_init;
        ops.read = &sio1_read;
        ops.write = &sio1_write;
        ops.flush = &sio1_flush;
        genv_tty_register(&ops);
    }

    int PSXSystem::init()
    {
        IRQ_MASK = 0;
        IRQ_STAT = 0;
        DMA_DPCR = 0;
        DMA_DICR = DMA_DICR_CH_STAT_BITMASK;
        cop0_setSR(COP0_SR_Im2 | COP0_SR_CU0 | COP0_SR_CU2);

        if (initIO() != 0)
        {
            return PSX_SYS_IO_INIT_FAIL;
        }
        if (initVideo() != 0)
        {
            return PSX_SYS_VIDEO_INIT_FAIL;
        }
        if (initFiles() != 0)
        {
            return PSX_SYS_FILE_INIT_FAIL;
        }
        if (initAudio() != 0)
        {
            return PSX_SYS_SOUND_INIT_FAIL;
        }

        psx_enableInterrupts();

        return PSX_SYS_OK;
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
        return 0;
    }

    int PSXSystem::initFiles()
    {
        return 0;

    }

    int PSXSystem::initIO()
    {
        sio1_init(115200);
        return 0;
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