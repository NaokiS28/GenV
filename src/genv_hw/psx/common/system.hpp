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
#include <atomic>

#include "common/services/system/iface_system.hpp"
#include "drivers/video/video.hpp"
#include "psx/common/drivers/pcdrive/psx_pcdrv.hpp"
#include "psx/common/drivers/sio1/psx_sio1.hpp"
#include "psx/common/system/registers.h"
#include "psx/common/system/sys.h"
#include "system/timers.hpp"

#include "common/services/system/timer.hpp"
#include "common/services/system/rtc/iface_rtc.hpp"
#include "common/services/system/system.hpp"

#include "drivers/sio0/psx_joy.hpp"
#include "drivers/sio0/psx_mc.hpp"
#include "drivers/sio0/psx_sio0.hpp"

namespace PS1
{
    enum
    {
        PS1_SYS_OK,
        PS1_SYS_VIDEO_INIT_FAIL,
        PS1_SYS_SOUND_INIT_FAIL,
        PS1_SYS_CDROM_INIT_FAIL,
        PS1_SYS_FILE_INIT_FAIL,
        PS1_SYS_IO_INIT_FAIL,
    };

    constexpr int maxRegisteredISRs = 5;

    // Test input parameter as an if statment, and if the process fails the test,
    // print an error string and associated error code.
    int ioTest(void *ptr, const char *device, const char *string);

    // Test input parameter as an if statment, and if the process fails the test,
    // print an error string and associated error code.
    int ioTest(int returnVal, const char *device, const char *string);

    // Test input parameter as an if statment, and if the process fails the test,
    // print an error string and associated error code and port number. Requires
    // `int error` to be defined in the context of this declaration. Will auto
    int ioTest(void *ptr, const char *device, int port, const char *string);

    // Test input parameter as an if statment, and if the process fails the test,
    // print an error string and associated error code and port number. Requires
    // `int error` to be defined in the context of this declaration. Will auto
    int ioTest(int returnVal, const char *device, int port, const char *string);

    class IRQCallback : public System::Callback
    {
    private:
        bool _ack = true;

    public:
        IRQCallback() {}
        IRQCallback(
            const char *name,
            System::CallbackFunction func,
            bool autoAck,
            void *arg) : System::Callback(name, func, arg), _ack(autoAck) {}

        inline bool autoAck()
        {
            // If not valid, handle the IRQ by clearing the interrupt
            return (!isValid() && _ack);
        }
    };

    /*
     * PS1 System base class
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
    class BasePS1System : public System::BaseSystem
    {
    protected:
        Time::IRTC *clock; // Pointer so can be overidden
        uint8_t sm_state;  // System Manager state for returning to main.cpp

        void interruptHandler();

        void isr_vsync_();
        void isr_timer2_();
        struct
        {
            uint32_t timer;
            System::Callback callback;
        } timer_alarms[2];

        IRQCallback isr_sio0;
        IRQCallback isr_sio1;
        IRQCallback isr_cdrom;
        IRQCallback isr_spu;
        IRQCallback isr_pio;

        // Millis/Seconds tracking
        const size_t err_numerator      = 307;
        const size_t err_denominator    = 512;
        volatile size_t timer2_addcycle = 0;
        volatile size_t timer2_count    = 0;
        volatile size_t timer2_erracc   = 0; // Time sync Error accumulator
        volatile size_t lastRTCTick     = 0;
        volatile bool doRTCtick         = false;

        System::SystemInfo siPS1 = {
            .type  = System::SYS_Console,
            .make  = szSony,
            .name  = szPlaystation,
            .flags = System::SYS_No_Window_Mode};

        GPU::PS1GPU *gpu = nullptr; // GPU probably needs to stay as pointer for V0/V2 GPU differences
        IO::SIO0_Bus sio0{*this};
        IO::SIO1_Bus sio1{*this};
        IO::PS1_Joypad joyDriver[2]    = {{*this, &sio0, IO::SIO0_Port::PORT1}, {*this, &sio0, IO::SIO0_Port::PORT2}}; // <-| These are part of the CPU and thus can always be "present"
        IO::PS1_MemoryCard mcDriver[2] = {{*this, &sio0, IO::SIO0_Port::PORT1}, {*this, &sio0, IO::SIO0_Port::PORT2}}; // <-/

#ifndef NDEBUG
        Storage::PS1_PCDrive *pcDriver = nullptr; // Not always needed?
#endif
    public:
        BasePS1System(ServiceManager &services);
        virtual ~BasePS1System();

        virtual int initCore() override;
        virtual int initVideo() override;
        virtual int initAudio() override { return 0; };
        virtual int initIO() override;
        virtual int initStorage() override;

        virtual int update() override;
        virtual bool setResolution(int w, int h); // Sets window resolution (internal viewport)

        inline virtual const System::SystemInfo *getSysInfo() const override
        {
            return &siPS1;
        }

        size_t millis() override
        {
            std::atomic_signal_fence(std::memory_order_acquire);
            constexpr int tmult = 5;
            constexpr int tdiv  = 21168;
            static_assert(((Timer2::ClockFreq * tmult) / tdiv) == 1000, "");

            return (uint64_t(::PS1::Timer2::Value | (timer2_count << 16)) * uint64_t(tmult)) / uint64_t(tdiv);
        }

        size_t micros() override
        {
            std::atomic_signal_fence(std::memory_order_acquire);
            constexpr int tmult = 625;
            constexpr int tdiv  = 2646;
            static_assert(((uint64_t(Timer2::ClockFreq) * tmult) / tdiv) == 1000000, "");

            return (uint64_t(::PS1::Timer2::Value | (timer2_count << 16)) * uint64_t(tmult)) / uint64_t(tdiv);
        }

        bool getTime(tm &time) override
        {
            if (clock)
                return clock->getTime(time);
            else
                return false;
        }

        virtual const char *getWorkingDirectory() override;

        inline bool registerTimerFunc(TFunc func, System::TChannel timer, uint8_t freq) override
        {
            // if (timer == TChannel::TIMER1)
            // return sysTimer.registerFunction(func, freq);
            return false;
        }
        inline bool unregisterTimerFunc(TFunc func, System::TChannel timer) override
        {
            // if (timer == TChannel::TIMER1)
            // return sysTimer.unregisterFunction(func);
            return false;
        }

        inline void enterCriticalSection() override { psx_disableInterrupts(); }
        inline void leaveCriticalSection() override { psx_enableInterrupts(); }

        IRQChannel registerISR(
            IRQChannel irq,
            const char *name,
            System::CallbackFunction func,
            bool autoAck = true,
            void *arg    = nullptr);
    };

} // namespace PS1