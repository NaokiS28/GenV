/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * window-mgr.hpp - Created on 25-04-2025
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
#include <vector>
#include "common/services/video/video.hpp"
#include "common/util/rect.h"

class WindowObject
{
public:
    HWND hWnd;
    HWND *hParentWnd;
    HINSTANCE hInst;
    DWORD dwStyle;
    DWORD dwExStyle;
    LPCSTR lpWindowName;
    int iIdx;

    Video::FullscreenMode fsMode;

    RectWH size;

    WindowObject(HINSTANCE hInst, DWORD style) { WindowObject(hInst, style, 640, 480, CW_USEDEFAULT, CW_USEDEFAULT); }
    WindowObject(HINSTANCE hInst, DWORD style, int w, int h) { WindowObject(hInst, style, w, h, CW_USEDEFAULT, CW_USEDEFAULT); }
    WindowObject(HINSTANCE hInst, DWORD style, int w, int h, int x, int y);
    ~WindowObject();

    bool Show();
    bool Resize(int w, int h, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT);
    bool SetWindowMode(Video::FullscreenMode mode, int w, int h, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT);
};

class WindowManager
{
private:
    std::vector<WindowObject*> vWindowList;

public:
    // DWORD dwExStyle,LPCSTR lpClassName,LPCSTR lpWindowName,DWORD dwStyle,int X,int Y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam
    WindowObject *NewWindow(HINSTANCE hInst, int w = 800, int h = 600, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT);
    WindowObject *NewScreen(HINSTANCE hInst, int w = 800, int h = 600, int x = 0, int y = 0);
    WindowObject *GetWindow(HWND hWnd);
    bool DestroyWindow(WindowObject *wObj);
    bool DestroyWindow(HWND hWnd);
    void shutdown();
};