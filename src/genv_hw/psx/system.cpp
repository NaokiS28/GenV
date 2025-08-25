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

#include <stdio.h>
#include <assert.h>

#include "system.hpp"
#include "psx/system/timer.h"
#include "registers.hpp"

#include "common/services/services.hpp"
#include "common/logger/log.hpp"
#include "common/util/misc.hpp"
#include "common/util/templates.hpp"
#include "system/serial.h"
#include "terminal/terminal.h"

namespace System::PSX
{

    PSXSystem::PSXSystem() : sm_state(SM_NORMAL)
    {
        psx_installExceptionHandler();
        GenV_ConsoleOps ops;
        ops.init = &sio1_init;
        ops.read = &sio1_read;
        ops.write = &sio1_write;
        ops.flush = &sio1_flush;
        genv_tty_register(&ops);

        clock = new Time::SoftRTC;
    }

    PSXSystem::~PSXSystem()
    {
        psx_uninstallExceptionHandler();
        if (clock)
            delete clock;
    }

    int PSXSystem::init()
    {
        psx_setInterruptHandler(util::forcedCast<ArgFunction>(&PSXSystem::_interruptHandler), this);

        if (_initIO() != 0)
        {
            return PSX_SYS_IO_INIT_FAIL;
        }
        if (_initVideo() != 0)
        {
            return PSX_SYS_VIDEO_INIT_FAIL;
        }
        if (_initFiles() != 0)
        {
            return PSX_SYS_FILE_INIT_FAIL;
        }
        if (_initAudio() != 0)
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

    int PSXSystem::_initVideo()
    {
        gpu = new GPU::PSXGPU;
        if (!gpu || gpu->init())
            return -1;
        Services::setVideo(adminKey, gpu);
        return 0;
    }

    int PSXSystem::_initAudio()
    {
        // IAudio *aDriver = Win32::CreateAudioDriver(Win32::AD_WIN_DSOUND,
        // gpuWnd); if (!aDriver || !aDriver->init())
        return 0;
    }

    int PSXSystem::_initFiles()
    {
        int r = 0;
        cdDriver = new PSX_CDROM();
        if (!cdDriver)
            r = -1;
        mcDriver = new PSX_MemCard();
        if (!mcDriver)
            r = -2;

#ifndef NDEBUG
        pcDriver = new PSX_PCDrive();
        if (!mcDriver)
            r = -3;
#endif
        return r;
    }

    int PSXSystem::_initIO()
    {
        sio1_init(115200);
        psx_timer_set_params(PSX_TIMER_0, PSX_TMR0_CLK_SRC_SYSTEM);
        psx_timer_set_params(PSX_TIMER_1, PSX_TMR2_CLK_SRC_SYSTEM);
        psx_timer_set_params(PSX_TIMER_2,
                             0 | PSX_TMR2_CLK_SRC_SYS_DIV8 +
                                     (PSX_TMR_RESET_ON_OVERFLOW | PSX_TMR_IRQ_ON_OVERFLOW |
                                      PSX_TMR_IRQ_REPEAT | PSX_TMR_IRQ_PULSE_BIT_10));
        psx_timer_enable_irq(PSX_TIMER_2);
        psx_timer_reset(PSX_TIMER_0);
        psx_timer_reset(PSX_TIMER_1);
        psx_timer_reset(PSX_TIMER_2);

        joyDriver = new PSX_Joypad();
        if (!joyDriver)
            return -1;
        Services::addInputDevice(joyDriver);

        return 0;
    }

    void PSXSystem::_interruptHandler(void)
    {
        std::atomic_signal_fence(std::memory_order_acquire);
        if (psx_acknowledgeInterrupt(IRQ_VSYNC))
        {
            gpu->waitingForVsync = false;
        }
        else if (psx_acknowledgeInterrupt(IRQ_TIMER2))
        {
            _isr_timer2();
        }
        std::atomic_signal_fence(std::memory_order_release);
    }

    void PSXSystem::_isr_timer2()
    {
        psx_timer_ack_irq(PSX_TIMER_2);
        // Timer 2 is used for millis/seconds but will drift out of sync
        //  as the timer is not a perfect division of time for seconds.
        //  This will account for this and add an extra every so often to
        //  correct for this.
        timer2_count += 1;
        if ((timer2_count - lastRTCTick) >= (64 + timer2_addcycle))
        {
            timer2_erracc += err_numerator;
            if (timer2_erracc >= err_denominator)
            {
                timer2_erracc -= err_denominator;
                timer2_addcycle += 1;
            }
            else
                timer2_addcycle = 0;
            lastRTCTick = timer2_count;
            if (clock)
                clock->tick();
        }
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

    const char *PSXSystem::getWorkingDirectory()
    {
        return nullptr;
    }

} // namespace System::PSX