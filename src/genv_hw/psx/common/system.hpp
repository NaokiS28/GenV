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
#include "psx/common/drivers/sio0/psx_sio0.hpp"
#include "system/timer.h"

#include "common/services/system/timer.hpp"
#include "common/services/system/rtc/iface_rtc.hpp"
#include "common/services/system/system.hpp"

#include "drivers/sio0/psx_joy.hpp"
#include "drivers/sio0/psx_mc.hpp"

#include "psx_strings.hpp"
#include "registers.hpp"

namespace System
{
    namespace PSX
    {
        enum
        {
            PSX_SYS_OK,
            PSX_SYS_VIDEO_INIT_FAIL,
            PSX_SYS_SOUND_INIT_FAIL,
            PSX_SYS_CDROM_INIT_FAIL,
            PSX_SYS_FILE_INIT_FAIL,
            PSX_SYS_IO_INIT_FAIL,
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
        class BasePSXSystem : public BaseSystem
        {
        protected:
            Time::IRTC *clock; // Pointer so can be overidden
            uint8_t sm_state;  // System Manager state for returning to main.cpp

            void setupInterruptHandler_(void);
            void interruptHandler_();

            void isr_vsync_();
            void isr_timer2_();

            Callback isr_sio0;

            // Millis/Seconds tracking
            const size_t err_numerator = 307;
            const size_t err_denominator = 512;
            volatile size_t timer2_addcycle = 0;
            volatile size_t timer2_count = 0;
            volatile size_t timer2_erracc = 0; // Time sync Error accumulator
            volatile size_t lastRTCTick = 0;

            SystemInfo siPS1 = {
                .type = SYS_Console,
                .make = szSony,
                .name = szPlaystation,
                .flags = SYS_No_Window_Mode};

            // Pointers to control the life cycle of items. TODO: Do these strictly *need* to be pointers?
            GPU::PSXGPU *gpu = nullptr;                                                     // GPU probably needs to stay as pointer for V1/V2 CPU differences
            IO::PSX_Joypad joyDriver[2] = {(IO::SIO0_Port::PORT1), (IO::SIO0_Port::PORT2)}; // <-| These are part of the CPU and thus can always be "present"
            IO::PSX_MemoryCard mcDriver[2] = {(1), (2)};                                    // <-/

        public:
            BasePSXSystem();
            virtual ~BasePSXSystem();

            virtual int initCore() override;
            virtual int initVideo() override;
            virtual int initAudio() override;
            virtual int initIO() override;
            virtual int initStorage() override;

            virtual int update() override;
            virtual bool shutdown() override;         // Prepare drivers and app for close
            virtual bool setResolution(int w, int h); // Sets window resolution (internal viewport)

            inline bool setFullscreen(Video::FullscreenMode mode) { return false; }
            inline bool toggleFullscreen() { return false; }

            inline virtual const SystemInfo *getSysInfo() const override
            {
                return &siPS1;
            }

            size_t millis() override
            {
                std::atomic_signal_fence(std::memory_order_acquire);
                constexpr int tmult = 5;
                constexpr int tdiv = 21168;
                static_assert(((TIMER2_FREQ * tmult) / tdiv) == 1000, "");

                return (uint64_t(TIMER_VALUE(PSX_TIMER_2) | (timer2_count << 16)) * uint64_t(tmult)) / uint64_t(tdiv);
            }

            bool getTime(tm &time) override
            {
                if (clock)
                    return clock->getTime(time);
                else
                    return false;
            }

            virtual const char *getWorkingDirectory() override;

            inline bool registerTimerFunc(TFunc func, TChannel timer, uint8_t freq) override
            {
                // if (timer == TChannel::TIMER1)
                // return sysTimer.registerFunction(func, freq);
                return false;
            }
            inline bool unregisterTimerFunc(TFunc func, TChannel timer) override
            {
                // if (timer == TChannel::TIMER1)
                // return sysTimer.unregisterFunction(func);
                return false;
            }

            IRQChannel registerISR(System::Callback callback, IRQChannel irq);
        };

    } // namespace PSX
} // namespace System