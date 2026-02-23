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

#include <atomic>

#include "system.hpp"
#include "common/return_codes.hpp"
#include "common/services/services.hpp"
#include "common/services/system/rtc/soft_rtc.hpp"

#include "psx_strings.hpp"
#include "system/pcsxhw.h"
#include "system/sys.h"
#include "system/timer.h"
#include "registers.hpp"

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

    BasePSXSystem::BasePSXSystem()
        : sm_state(SM_NORMAL)
    {
        // TODO: SIO1 driver will require interrupts in future, so this will need to change.
        // We need to do this here (or change the boot process in GenV) so that boot logs are written.
        // Suggest using PSX BIOS puts/gets as PSX BIOS handler is still running at this point.
        GenV_TerminalFuncs ops;
        ops.init  = &sio1_init;
        ops.read  = &sio1_read;
        ops.write = &sio1_write;
        ops.flush = &sio1_flush;
        genv_tty_register(&ops);

        clock = new Time::SoftRTC; // New clock here so GenV boot logs have correct timestamps
    }

    BasePSXSystem::~BasePSXSystem()
    {
        // Restore the PSX BIOS handler (Unlikely to happen, but just in case)
        psx_uninstallExceptionHandler();
        if (clock) delete clock;
    }

    int BasePSXSystem::initCore()
    {
        if (pcsx_present()) LOG_SYS(szRedux);

        // Prior to this point, we are using the Sony BIOS for interrupts
        psx_installExceptionHandler();
        psx_setInterruptHandler(
            [](void *arg)
            {
                auto app = reinterpret_cast<BasePSXSystem *>(arg);
                app->interruptHandler(); // etc.
            },
            this);

        psx_enableInterrupts();
        return PSX_SYS_OK;
    }

    bool BasePSXSystem::setResolution(int w, int h)
    {
        if (!gpu)
            return false;

        sm_state = SM_RESIZE;
        return gpu->setResolution(w, h);
    }

    int BasePSXSystem::initVideo()
    {
        int error = 0;
        gpu       = new GPU::PSXGPU();
        error     = ioTest(gpu, PSX_GPU_STR, PSX_CREATE_STR);
        if (!error) ioTest(gpu->init(), PSX_GPU_STR, PSX_INIT_STR);
        if (!error) services.setVideo(adminKey, gpu);
        return error;
    }

    int BasePSXSystem::initAudio()
    {
        /*
        int error = 0;
        spu       = new Sound::PSXSPU;
        error     = ioTest(spu, PSX_SPU_STR, PSX_CREATE_STR);
        if (!error) ioTest(spu->init(), PSX_SPU_STR, PSX_INIT_STR);
        if (!error) services.setAudio(adminKey, spu);
        */
        return 0;
    }

    int BasePSXSystem::initStorage()
    {
        int error = 0; // TODO: How to handle multiple driver failures?

        int port = 1;
        for (auto &mc : mcDriver)
        {
            int mcError = ioTest(mc.init(), PSX_MEMORY_CARD_STR, port, PSX_INIT_STR);
            if (!mcError) services.registerDriver(&mc);
        }

        return error;
    }

    int BasePSXSystem::initIO()
    {
        // TODO: Allow setting custom startup baud
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

        sio0.init();

        int error = 0;
        int port  = 1;
        for (auto &joy : joyDriver)
        {
            error = ioTest(joy.init(), PSX_JOYPAD_STR, port++, PSX_INIT_STR);
            if (!error) services.registerDriver(&joy);
        }

        return error;
    }

    void BasePSXSystem::interruptHandler(void)
    {
        if (psx_testInterrupt(IRQ_VSYNC, true)) isr_vsync_();
        if (psx_testInterrupt(IRQ_TIMER2, true)) isr_timer2_();
        if (psx_testInterrupt(IRQ_SIO0, isr_sio0_autoAck) && isr_sio0.isValid()) isr_sio0.call();
    }

    void BasePSXSystem::isr_vsync_()
    {
        std::atomic_signal_fence(std::memory_order_acquire);
        gpu->_waitingForVsync = false;
        gpu->_frameCount++;
        std::atomic_signal_fence(std::memory_order_release);
    }

    void BasePSXSystem::isr_timer2_()
    {
        std::atomic_signal_fence(std::memory_order_acquire);
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
            doRTCtick   = true;
        }
        std::atomic_signal_fence(std::memory_order_release);
    }

    int BasePSXSystem::update()
    {
        sm_state = SM_NORMAL;
        if (doRTCtick && clock)
        {
            clock->tick();
            doRTCtick = false;
        }
        return sm_state;
    }

    bool BasePSXSystem::shutdown()
    {
        return true;
    }

    const char *BasePSXSystem::getWorkingDirectory()
    {
        // Figure out how to get a path which the program started at, more often than not ODD0:
        return nullptr;
    }

    IRQChannel BasePSXSystem::registerISR(System::Callback callback, IRQChannel irq, bool autoAck)
    {
        switch (irq)
        {
        case IRQ_SIO0:
            isr_sio0         = callback;
            isr_sio0_autoAck = autoAck;
            return (callback.isValid() ? irq : IRQ_INVALID);
        default: return IRQ_INVALID;
        }
    }

} // namespace System::PSX