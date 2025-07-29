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

#include <windows.h>
#include <tchar.h>
#include <stdbool.h>

#include "common/services/services.hpp"
#include "common/services/sys/timer.hpp"

// Win32
#include "video/video.hpp"
#include "audio/audio.hpp"
#include "file/file.hpp"

#include "window-mgr.hpp"
#include "targetver.h"

namespace System
{
    // Windows process message handler
    namespace WinAPI
    {
        LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
        VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN);
    }

    class WinTimer : public Timer
    {
    public:
        HANDLE timerHandle = nullptr;
    };

    // Windows specific system manager interface.
    class WinSystem : public ISystem
    {
    private:
        WindowManager winManager;
        WinStorage storage;

        WindowObject *gpuWnd = nullptr;
        HINSTANCE hInst;

        LARGE_INTEGER freq;
        LARGE_INTEGER start;
        WinTimer sysTimer;

        uint8_t sm_state; // System Manager state for returning to main.cpp

        bool initWindowClass();
        int initVideo();
        int initAudio();
        int initIO() { return 0; }
        int initFiles();

        inline bool linkServices()
        {
            assert(this != nullptr);
            Services::setSystem(this);
            return true;
        }

        SystemInfo siWindows;

    public:
        WinSystem() : sm_state(System::SM_NORMAL)
        {
            QueryPerformanceFrequency(&freq);
            QueryPerformanceCounter(&start);
        }
        ~WinSystem() {}

        bool init() override;             // Registers Windows app class and inits drivers
        int update() override;            // Process wWindows messages
        bool shutdown() override;         // Prepare drivers and app for close
        bool setResolution(int w, int h); // Sets window resolution (internal viewport)
        bool setFullscreen(Video::FullscreenMode mode);
        bool toggleFullscreen();

        const SystemInfo* getSysInfo() const override {
            return &siWindows;
        }

        size_t millis();

        inline bool registerTimerFunc(TFunc func, TChannel timer, uint8_t freq)
        {
            if (timer == TChannel::TIMER1)
                return sysTimer.registerFunction(func, freq);
            return false;
        }
        inline bool unregisterTimerFunc(TFunc func, TChannel timer)
        {
            if (timer == TChannel::TIMER1)
                return sysTimer.unregisterFunction(func);
            return false;
        }

        inline void hardwareChanged(WPARAM wParam, LPARAM lParam){
            storage.hardwareChanged(wParam, lParam);
        }
    };

}
