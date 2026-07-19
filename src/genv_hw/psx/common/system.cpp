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

#include "common/services/system/system.hpp"
#include "psx/common/system/registers.h"
#include "psx_strings.hpp"
#include "system/pcsxhw.h"
#include "system/sys.h"
#include "system/timers.hpp"

#include "system/serial.h"
#include "terminal/terminal.h"

namespace PS1
{

    int ioTest(void *ptr, const char *device, const char *string)
    {
        if (ptr == nullptr)
        {
            LOG_SYS(PS1_IO_ERROR_FMT, string, device);
            return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_CREATE_FAILED);
        }
        return GV_OK;
    }

    int ioTest(int returnVal, const char *device, const char *string)
    {
        if (returnVal != GV_OK)
        {
            LOG_SYS(PS1_IO_ERROR_FMT, string, device);
        }
        return returnVal;
    }

    int ioTest(void *ptr, const char *device, int port, const char *string)
    {
        if (ptr == nullptr)
        {
            LOG_SYS(PS1_IO_ERROR_FMT, string, device, port);
            return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_CREATE_FAILED);
        }
        return GV_OK;
    }

    int ioTest(int returnVal, const char *device, int port, const char *string)
    {
        if (returnVal != GV_OK)
        {
            LOG_SYS(PS1_IO_ERROR_FMT, string, device, port);
        }
        return returnVal;
    }

    BasePS1System::BasePS1System(ServiceManager &services)
        : System::BaseSystem(services),
          sm_state(System::SM_NORMAL)
    {
        // TODO: SIO1 driver will require interrupts in future, so this will need to change.
        // We need to do this here (or change the boot process in GenV) so that boot logs are written.
        // Suggest using PS1 BIOS puts/gets as PS1 BIOS handler is still running at this point.
        GenV_TerminalFuncs tty_ops;
        tty_ops.init  = &sio1_init;
        tty_ops.read  = &sio1_read;
        tty_ops.write = &sio1_write;
        tty_ops.flush = &sio1_flush;
        genv_tty_register(&tty_ops);

        clock = new Time::SoftRTC; // New clock here so GenV boot logs have correct timestamps
    }

    BasePS1System::~BasePS1System()
    {
        // Restore the PS1 BIOS handler (Unlikely to happen, but just in case)
        psx_uninstallExceptionHandler();
        if (clock) delete clock;
    }

    int BasePS1System::initCore()
    {
        System::BaseSystem::initCore();
        if (pcsx_present()) LOG_SYS(szRedux);

        // Prior to this point, we are using the Sony BIOS for interrupts
        psx_installExceptionHandler();
        psx_setInterruptHandler(
            [](void *arg)
            {
                auto app = reinterpret_cast<BasePS1System *>(arg);
                app->interruptHandler(); // etc.
            },
            this);

        psx_enableInterrupts();
        return GV_OK;
    }

    bool BasePS1System::setResolution(int w, int h)
    {
        if (!gpu)
            return false;

        sm_state = System::SM_RESIZE;
        return true; // gpu->setResolution(w, h); // RIX
    }

    int BasePS1System::initVideo()
    {
        int error = 0;
        // GPU driver injects *this and allocates its own screen 0 in init()
        // (via System::assignScreen); see PS1GPU::init().
        gpu   = new GPU::PS1GPU(*this); // Inits a V2 GPU
        error = ioTest(gpu, PS1_GPU_STR, PS1_CREATE_STR);
        if (!error) ioTest(registerDriver(gpu), PS1_GPU_STR, PS1_INIT_STR);
        return error;
    }

    int BasePS1System::initStorage()
    {
        int error = 0;
        int port  = 1;
        for (auto &mc : mcDriver)
            error = ioTest(
                registerDriver(&mc) ? GV_OK : 1,
                PS1_PS_MEMCARD_STR, port++, PS1_INIT_STR);

#ifndef NDEBUG
        int pcError = 0;
        pcDriver    = new Storage::PS1_PCDrive(*this);
        pcError     = ioTest(pcDriver, PS1_PC_DRIVE_STR, PS1_CREATE_STR);
        if (!pcError) registerDriver(pcDriver);
#endif

        return error;
    }

    int BasePS1System::initIO()
    {
        // TODO: Allow setting custom startup baud - This is a GenV common issue
        sio1_init(115200);
        Timer0::Ctrl = (uint16_t)Timer0::ClockSource::SYSTEM;
        Timer1::Ctrl = (uint16_t)Timer1::ClockSource::SYSTEM;
        Timer2::Ctrl = (uint16_t)Timer2::ClockSource::SYS_DIV8 |
                       CTRL_IRQ_ON_OVERFLOW | CTRL_IRQ_REPEAT | CTRL_IRQ_TOGGLE;
        IRQ_MASK |= 1 << IRQ_TIMER2;
        Timer0::Value = 0;
        Timer1::Value = 0;
        Timer2::Value = 0;

        registerDriver(&sio0);
        registerDriver(&sio1);

        int error = 0;
        int port  = 1;
        for (auto &joy : joyDriver)
            error = ioTest(
                registerDriver(&joy) ? GV_OK : 1,
                PS1_JOYPAD_STR, port++, PS1_INIT_STR);

        return error;
    }

    void BasePS1System::interruptHandler(void)
    {
        if (psx_testInterrupt(IRQ_VSYNC, true)) isr_vsync_();
        if (psx_testInterrupt(IRQ_TIMER2, true)) isr_timer2_();
        // if (psx_testInterrupt(IRQ_DMA, true)) isr_dma_();
        if (psx_testInterrupt(IRQ_CDROM, isr_cdrom.autoAck()) && isr_cdrom.isValid()) isr_cdrom.call();
        if (psx_testInterrupt(IRQ_SIO0, isr_sio0.autoAck()) && isr_sio0.isValid()) isr_sio0.call();
        if (psx_testInterrupt(IRQ_SIO1, isr_sio1.autoAck()) && isr_sio1.isValid()) isr_sio1.call();
        if (psx_testInterrupt(IRQ_SPU, isr_spu.autoAck()) && isr_spu.isValid()) isr_spu.call();
        if (psx_testInterrupt(IRQ_PIO, isr_pio.autoAck()) && isr_pio.isValid()) isr_pio.call();
    }

    void BasePS1System::isr_vsync_()
    {
        std::atomic_signal_fence(std::memory_order_acquire);
        gpu->_waitingForVsync = false;
        gpu->_frameCount++;
        std::atomic_signal_fence(std::memory_order_release);
    }

    void BasePS1System::isr_timer2_()
    {
        std::atomic_signal_fence(std::memory_order_acquire);
        ::PS1::Timer2::Ctrl |= ::PS1::CTRL_ACK_IRQ;
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

    int BasePS1System::update()
    {
        System::BaseSystem::update();
        sm_state = System::SM_NORMAL;
        if (doRTCtick && clock)
        {
            clock->tick();
            doRTCtick = false;
        }
        return sm_state;
    }

    const char *BasePS1System::getWorkingDirectory()
    {
        // Figure out how to get a path which the program started at, more often than not ODD0:
        return nullptr;
    }

    IRQChannel BasePS1System::registerISR(
        IRQChannel irq,
        const char *name,
        System::CallbackFunction func,
        bool autoAck,
        void *arg)
    {
        if (func == nullptr) return IRQ_INVALID;
        switch (irq)
        {
        case IRQ_SIO0: isr_sio0 = IRQCallback(name, func, autoAck, arg); break;
        case IRQ_SIO1: isr_sio1 = IRQCallback(name, func, autoAck, arg); break;
        case IRQ_CDROM: isr_cdrom = IRQCallback(name, func, autoAck, arg); break;
        case IRQ_SPU: isr_spu = IRQCallback(name, func, autoAck, arg); break;
        case IRQ_PIO: isr_pio = IRQCallback(name, func, autoAck, arg); break;
        default: return IRQ_INVALID;
        }
        return irq;
    }
} // namespace PS1