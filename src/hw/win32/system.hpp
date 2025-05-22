/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * system.hpp - Created on 25-04-2025
 *
 * DXUX is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * DXUX is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * DXUX. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <windows.h>
#include <tchar.h>
#include <stdbool.h>

#include "common/services/services.hpp"
#include "video/video.hpp"

#include "window-mgr.hpp"
#include "targetver.h"

namespace System
{
    // Windows process message handler
    LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

    // Windows specific system manager interface.
    class WinSystem : public ISystem
    {
    private:
        WindowManager winManager;
        WinVideo video;

        WindowObject *gpuWnd;
        HINSTANCE hInst;

        uint8_t sm_state;           // System Manager state for returning to main.cpp

        int initVideo();
        int initAudio(){ return 0; }
        int initIO(){ return 0; }

    public:
        WinSystem() : sm_state(System::SM_NORMAL) {}
        ~WinSystem() {}

        bool init() override;               // Registers Windows app class and inits drivers
        int update() override;              // Process wWindows messages
        bool shutdown() override;           // Prepare drivers and app for close
        bool setResolution(int w, int h);   // Sets window resolution (internal viewport)
        bool setFullscreen(Video::FullscreenMode mode);
    };

}
