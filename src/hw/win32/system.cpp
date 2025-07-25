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
#include "resources.h"

#include "common/util/log.hpp"
#include "common/util/misc.hpp"

namespace System
{
    IWinVideo *video(){ return static_cast<IWinVideo *>(Services::getVideo()); }
    IAudio *audio(){ return Services::getAudio(); }

    bool WinSystem::init()
    {
        if (!initWindowClass())
            return false;
        SetProcessDPIAware();
        // You must set the system service before initing further drivers
        linkServices();

        WindowObject *wObj = winManager.NewWindow(hInst);
        if (!wObj)
            return 1;

        gpuWnd = wObj;

        if (initVideo())
        {
            return 1;
        }
        if (initFiles())
        {
            return 1;
        }
        if (initAudio())
        {
            return 1;
        }
        if (initIO())
        {
            return 1;
        }

        setFullscreen(Video::Windowed); // TODO: Fullscreen and Borderless are fucked up.
        return true;
    }

    bool WinSystem::setResolution(int w, int h)
    {
        if (!video())
            return false;

        sm_state = System::SM_RESIZE;
        return video()->setResolution(w, h);
    }

    bool WinSystem::setFullscreen(Video::FullscreenMode mode)
    {
        if (!video())
            return false;
        sm_state = System::SM_RESIZE;
        return         video()->setFullscreen(mode);
    }

    bool WinSystem::toggleFullscreen()
    {
        if (!video())
            return false;
        video()->toggleFullscreen();
        sm_state = System::SM_RESIZE;
        return true;
    }

    int WinSystem::initVideo()
    {
        if (!gpuWnd)
            return -1;

        IWinVideo *vDriver = Win32::CreateVideoDriver(Win32::VD_WIN_D3D, gpuWnd);
        if (!vDriver || !vDriver->init())
            return -2;

        Services::setVideo(vDriver);
        video()->setResolution(gpuWnd->size.w, gpuWnd->size.h);
        return 0;
    }

    int WinSystem::initAudio()
    {
        if (!gpuWnd)
            return -1;

        IAudio *aDriver = Win32::CreateAudioDriver(Win32::AD_WIN_DSOUND, gpuWnd);
        if (!aDriver || !aDriver->init())
            return -2;

        Services::setAudio(aDriver);
        return 0;
    }

    int WinSystem::initFiles()
    {
        storage.setWindow(gpuWnd);
        if(!storage.init())
            return -2;

        Services::setStorage(&storage);
        return 0;
    }

    int WinSystem::update()
    {
        sm_state = System::SM_NORMAL;

        MSG msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
        {
            return System::SM_QUIT;
        }

        return sm_state;
    }

    bool WinSystem::shutdown()
    {
        DeleteTimerQueueTimer(NULL, sysTimer.timerHandle, NULL); // Stop 10ms timer
        winManager.shutdown();
        UnregisterClass(szWindowClass, hInst);
        return true;
    }

    size_t WinSystem::millis()
    {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return static_cast<size_t>(
            (now.QuadPart - start.QuadPart) * 1000 / freq.QuadPart);
    }

    bool WinSystem::initWindowClass()
    {
        hInst = GetModuleHandle(nullptr);

        WNDCLASSEX wcex;
        ZeroMemory(&wcex, sizeof(WNDCLASSEX));

        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WinAPI::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInst;
        wcex.hIcon = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GenVICON));
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = szWindowClass;
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GenVICON));

        if (!RegisterClassEx(&wcex))
        {
            LOG_APP("Critical error: Class registration failed");
            MessageBox(NULL,
                       _T("Critical error: Class registration failed"),
                       szAppName,
                       MB_OK);
            return false;
        }
        return true;
    }
}