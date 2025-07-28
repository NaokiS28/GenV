/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * win_platform.cpp - Created on 28-05-2025
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

#include <windows.h>
#include "system.hpp"

namespace System
{
    namespace WinAPI
    {
        // Windows message handling routine
        LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch (message)
            {
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
            case WM_SIZE:
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                static_cast<System::WinSystem *>(Services::getSystem())->setResolution(width, height);
            }
            break;
            case WM_KEYDOWN:
            {
                if (LOWORD(wParam) == VK_F11)
                {
                    static_cast<System::WinSystem *>(Services::getSystem())->toggleFullscreen();
                }
            }
            break;
            case WM_DEVICECHANGE:
            {
                static_cast<System::WinSystem *>(Services::getSystem())->hardwareChanged(wParam, lParam);
            }
            break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            return 0;
        }

        VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN)
        {
            System::Timer *timer = reinterpret_cast<System::Timer *>(lpParam);
            timer->tick();
        }
    }
}