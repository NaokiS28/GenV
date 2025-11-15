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
#include <atomic>

#include "system.hpp"
#include "common/return_codes.hpp"
#include "psx/psx_strings.hpp"
#include "psx/system/pcsxhw.h"
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

    int ioTest(void *ptr, const char *device, const char *string)
    {
        if (ptr == nullptr)
        {
            LOG_SYS(PSX_IO_ERROR_FMT, string, device);
            return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_CREATE_FAILED);
        }
        return GV_OK;
    }

    int ioTest(int returnVal, const char *device, const char *string)
    {
        if (returnVal != GV_OK)
        {
            LOG_SYS(PSX_IO_ERROR_FMT, string, device);
        }
        return returnVal;
    }

    int ioTest(void *ptr, const char *device, int port, const char *string)
    {
        if (ptr == nullptr)
        {
            LOG_SYS(PSX_IO_ERROR_FMT, string, device, port);
            return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_CREATE_FAILED);
        }
        return GV_OK;
    }

    int ioTest(int returnVal, const char *device, int port, const char *string)
    {
        if (returnVal != GV_OK)
        {
            LOG_SYS(PSX_IO_ERROR_FMT, string, device, port);
        }
        return returnVal;
    }

    PSXSystem::PSXSystem() : sm_state(SM_NORMAL)
    {
        psx_installExceptionHandler();
        GenV_TerminalFuncs ops;
        ops.init  = &sio1_init;
        ops.read  = &sio1_read;
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

    int PSXSystem::initCore()
    {
        if (pcsx_present())
        {
            LOG_SYS(szRedux);
        }
        _setupInterruptHandler();
        psx_enableInterrupts();
        return PSX_SYS_OK;
    }

    bool PSXSystem::setResolution(int w, int h)
    {
        if (!gpu)
            return false;

        sm_state = SM_RESIZE;
        return gpu->setResolution(w, h);
    }

    int PSXSystem::initVideo()
    {
        int error = 0;
        gpu       = new GPU::PSXGPU;
        error     = ioTest(gpu, PSX_GPU_STR, PSX_CREATE_STR);
        if (!error) ioTest(gpu->init(), PSX_GPU_STR, PSX_INIT_STR);
        if (!error) Services::setVideo(adminKey, gpu);
        return error;
    }

    int PSXSystem::initAudio()
    {
        /*
        int error = 0;
        spu       = new Sound::PSXSPU;
        error     = ioTest(spu, PSX_SPU_STR, PSX_CREATE_STR);
        if (!error) ioTest(spu->init(), PSX_SPU_STR, PSX_INIT_STR);
        if (!error) Services::setAudio(adminKey, spu);
        */
        return 0;
    }

    int PSXSystem::initStorage()
    {
        int error = 0; // TODO: How to handle multiple driver failures?
        cdDriver  = new Storage::PSX_CDROM();
        error     = ioTest(cdDriver, PSX_CDROM_DRIVE_STR, PSX_CREATE_STR);
        if (!error) error = ioTest(cdDriver->init(), PSX_CDROM_DRIVE_STR, PSX_INIT_STR);
        if (!error) Services::registerStorageDriver(cdDriver);

        int port = 1;
        for (auto &mc : mcDriver)
        {
            int mcError = ioTest(mc.init(), PSX_MEMORY_CARD_STR, port, PSX_INIT_STR);
            if (!mcError) Services::registerStorageDriver(&mc);
        }

#ifndef NDEBUG
        int pcError = 0;
        pcDriver    = new Storage::PSX_PCDrive();
        pcError     = ioTest(pcDriver, PSX_PC_DRIVE_STR, PSX_CREATE_STR);
        if (!pcError) pcError = ioTest(pcDriver->init(), PSX_PC_DRIVE_STR, PSX_INIT_STR);
        if (!pcError) Services::registerStorageDriver(pcDriver);
#endif
        return error;
    }

    int PSXSystem::initIO()
    {
        sio1_init(115200);
        psx_timer_set_params(PSX_TIMER_0, PSX_TMR0_CLK_SRC_SYSTEM);
        psx_timer_set_params(PSX_TIMER_1, PSX_TMR2_CLK_SRC_SYSTEM);
        psx_timer_set_params(PSX_TIMER_2,
                             (0 | PSX_TMR2_CLK_SRC_SYS_DIV8) +
                                 (PSX_TMR_RESET_ON_OVERFLOW | PSX_TMR_IRQ_ON_OVERFLOW |
                                  PSX_TMR_IRQ_REPEAT | PSX_TMR_IRQ_PULSE_BIT_10));
        psx_timer_enable_irq(PSX_TIMER_2);
        psx_timer_reset(PSX_TIMER_0);
        psx_timer_reset(PSX_TIMER_1);
        psx_timer_reset(PSX_TIMER_2);

        int error = 0;
        int port  = 1;
        for (auto &joy : joyDriver)
        {
            error = ioTest(joy.init(), PSX_JOYPAD_STR, port, PSX_INIT_STR);
            if (!error) Services::registerInputDriver(&joy);
            port++;
        }

        return error;
    }

    void PSXSystem::_setupInterruptHandler(void)
    {
        psx_setInterruptHandler(
            [](void *arg)
            {
                auto app = reinterpret_cast<PSXSystem *>(arg);
                app->_interruptHandler(); // etc.
            },
            this);
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
        // Figure out how to get a path which the program started at, more often than not ODD0:
        return nullptr;
    }

} // namespace System::PSX