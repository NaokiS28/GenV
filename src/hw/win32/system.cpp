/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * system.cpp - Created on 25-04-2025
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

#include "system.hpp"
#include "resources.h"

#include "common/util/log.hpp"

namespace System
{
    bool WinSystem::init()
    {
        hInst = GetModuleHandle(nullptr);

        WNDCLASSEX wcex;

        ZeroMemory(&wcex, sizeof(WNDCLASSEX));

        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInst;
        wcex.hIcon = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_DXUXICON));
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = szWindowClass;
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_DXUXICON));

        if (!RegisterClassEx(&wcex))
        {
            LOG_APP("Critical error: Class registration failed");
            MessageBox(NULL,
                       _T("Critical error: Class registration failed"),
                       szAppName,
                       MB_OK);
            return 1;
        }

        SetProcessDPIAware();
        Services::setSystem(this); // This needs to be set for subsequent drivers else they will segfault.

        if (initVideo() || initAudio() || initIO())
        {
            return 1;
        }

        setFullscreen(Video::Windowed);     // TODO: Fullscreen and Borderless are fucked up.

        return 0;
    }

    bool WinSystem::setResolution(int w, int h)
    {
        sm_state = System::SM_RESIZE;
        return video.setResolution(w, h);
    }

    bool WinSystem::setFullscreen(Video::FullscreenMode mode)
    {
        video.setFullscreen(mode);
        sm_state = System::SM_RELOAD;
        return true;
    }

    int WinSystem::initVideo()
    {
        WindowObject *wObj = winManager.NewWindow(hInst);
        if (!wObj)
            return 1;

        gpuWnd = wObj;

        video.init();
        video.setWindow(wObj);
        video.setResolution(wObj->size.w, wObj->size.h);

        Services::setVideo(video.getVideoService());

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
        video.shutdown();
        winManager.shutdown();
        UnregisterClass(szWindowClass, hInst);
        return 1;
    }

    // Windows message handling routine
    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
        {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            static_cast<WinSystem *>(Services::getSystem())->setResolution(width, height);
        }
            return 0;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
}