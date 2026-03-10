/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * system.hpp - Hello World example for macOS system manager
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

/*
 * HELLO WORLD EXAMPLE - system.hpp
 * ==================================
 * Declaration of the OSXSystem class, the macOS equivalent of WinSystem
 * (src/genv_hw/win32/system.hpp).
 *
 * Why void* instead of NSWindow* / id<...>?
 * -------------------------------------------
 * This header is a plain C++ header (.hpp).  If we put Objective-C types like
 * NSWindow* directly here, any C++ file that #includes it would fail to
 * compile because the compiler would need to be in Objective-C++ mode.
 *
 * The solution used across the codebase is to store ObjC objects as void* in
 * headers and cast them back to their real types inside the .mm implementation
 * files where the compiler is already in Objective-C++ mode.
 *
 * Note: mach_timebase_info_data_t IS a C struct (from <mach/mach_time.h>) and
 * is safe to include in a C++ header.
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include <mach/mach_time.h> // mach_absolute_time, mach_timebase_info

#include "common/services/services.hpp"
#include "common/services/system/system.hpp"
#include "common/services/system/iface_system.hpp"
#include "common/services/system/timer.hpp"

namespace System
{
    // -------------------------------------------------------------------------
    // OSXSystem
    // -------------------------------------------------------------------------
    // Implements the ISystem interface for macOS 11+.
    //
    // Lifetime (called by main.cpp / GenvSystemClass in the same order as
    // Win32's WinSystem):
    //   1. Constructor     – capture start time and fill in _sysInfo.
    //   2. initCore()      – create NSApplication, NSWindow.
    //   3. initVideo()     – create and init MetalGPU, register with services.
    //   4. initAudio()     – (stub) create CoreAudio driver.
    //   5. initIO()        – (stub) create keyboard/gamepad drivers.
    //   6. initStorage()   – (stub) set up file-system paths.
    //   7. update()        – pump NSApp event queue each tick.
    //   8. shutdown()      – release window and drivers.
    // -------------------------------------------------------------------------
    class OSXSystem : public BaseSystem
    {
    public:
        OSXSystem();
        ~OSXSystem() override;

        // ---- ISystem lifecycle -----------------------------------------------
        int initCore() override;
        int initVideo() override;
        int initAudio() override;
        int initIO() override;
        int initStorage() override;

        int update() override;
        void shutdown() override;

        void enterCriticalSection() override;
        void leaveCriticalSection() override;

        // ---- System information ---------------------------------------------
        const SystemInfo *getSysInfo() const override { return &_sysInfo; }

        // ---- Timing ---------------------------------------------------------
        size_t millis() override;
        size_t micros() override;
        bool getTime(tm &time) override;

        // ---- File system ----------------------------------------------------
        const char *getWorkingDirectory() override;

        // ---- Timer callbacks ------------------------------------------------
        bool registerTimerFunc(TFunc func, TChannel timer, uint8_t freq) override;
        bool unregisterTimerFunc(TFunc func, TChannel timer) override;

        // ---- Quit signalling ------------------------------------------------
        // Called by GenVAppDelegate when the OS requests termination so that
        // the main loop can shut down cleanly via the state machine.
        void requestQuit() { _smState = System::SM_QUIT; }

    private:
        // The NSWindow* and app delegate are stored as void* so this header
        // stays valid C++.  Objective-C types only appear inside system.mm.
        void *_window   = nullptr; // NSWindow*
        void *_delegate = nullptr; // GenVAppDelegate*

        char m_systemName[32]                     = {'\0'};
        char m_systemVersion[64]                  = {'\0'};
        static constexpr const char *m_systemMake = "Apple";

        // Monotonic timing state.
        mach_timebase_info_data_t _tbInfo = {};
        uint64_t _startTick               = 0;

        // System-manager state code returned to main.cpp each tick
        // (SM_NORMAL, SM_RESIZE, SM_QUIT, …).
        uint8_t _smState = System::SM_NORMAL;

        // Static system description filled in by the constructor.
        SystemInfo _sysInfo;

        // Software timer for registered callbacks (mirrors WinSystem::sysTimer).
        Timer _sysTimer;
    };

} // namespace System
