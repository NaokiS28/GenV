/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * windows-mgr.cpp - Created on 25-04-2025
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

#include <windows.h>
#include <stdexcept>

#include "window-mgr.hpp"
#include "resources.h"

WindowObject *WindowManager::NewWindow(HINSTANCE hInst, int w, int h, int x, int y)
{
    try
    {
        WindowObject *wObj = new WindowObject(hInst, WS_OVERLAPPEDWINDOW, w, h, x, y);
        vWindowList.push_back(wObj);
        wObj->iIdx = (int)vWindowList.size() - 1;
        wObj->Show();
        return wObj;
    }
    catch (std::runtime_error const &)
    {
        MessageBox(NULL,
                   "Critical Error: Window creation failed.",
                   szAppName, MB_OK);
        return nullptr;
    }
    return nullptr;
}

WindowObject *WindowManager::NewScreen(HINSTANCE hInst, int w, int h, int x, int y)
{
    try
    {
        WindowObject *wObj = new WindowObject(hInst, WS_POPUP, w, h, x, y);
        vWindowList.push_back(wObj);
        wObj->iIdx = (int)vWindowList.size() - 1;
        wObj->SetWindowMode(Video::Fullscreen, w, h, 0, 0);
        return wObj;
    }
    catch (std::runtime_error const &)
    {
        MessageBox(NULL,
                   "Critical Error: Window creation failed.",
                   szAppName, MB_OK);
        return nullptr;
    }
    return nullptr;
}

WindowObject *WindowManager::GetWindow(HWND hWnd)
{
    for (WindowObject *window : vWindowList)
    {
        if (window->hWnd == hWnd)
        {
            return window;
        }
    }
    return nullptr;
}

bool WindowManager::DestroyWindow(WindowObject *wObj)
{
    int idx = 0;
    for (WindowObject *window : vWindowList)
    {
        if (window == wObj)
        {
            delete window;
            vWindowList.erase(vWindowList.cbegin() + idx);
            return 0;
        }
        idx++;
    }
    return 1;
}

bool WindowManager::DestroyWindow(HWND hWnd)
{
    int idx = 0;
    for (WindowObject *window : vWindowList)
    {
        if (window->hWnd == hWnd)
        {
            delete window;
            vWindowList.erase(vWindowList.cbegin() + idx);
            return 0;
        }
        idx++;
    }
    return 1;
}

void WindowManager::shutdown()
{
    for (WindowObject *window : vWindowList)
    {
        delete window;
    }
    vWindowList.clear();
}

WindowObject::WindowObject(HINSTANCE hInst,
                           DWORD style,
                           int w, int h,
                           int x, int y) : hWnd(nullptr),
                                           hParentWnd(nullptr),
                                           hInst(hInst),
                                           dwStyle(style),
                                           size({x, y, w, h})
{
    RECT area = {0, 0, w, h};
    AdjustWindowRect(&area, dwStyle, false);
    hWnd = CreateWindow(
        szWindowClass,
        szAppName,
        style,
        x, y,
        area.right - area.left,
        area.bottom - area.top,
        NULL,
        NULL,
        hInst,
        NULL);

    if (!hWnd)
    {
        throw std::runtime_error("Failed to construct WindowObject.");
    }

    lpWindowName = szAppName;
    fsMode = Video::Windowed;
}

WindowObject::~WindowObject()
{
    if (hWnd)
    {
        DestroyWindow(hWnd);
    }
}

bool WindowObject::Show()
{
    ShowWindow(this->hWnd, SW_SHOW);
    UpdateWindow(this->hWnd);
    return true;
}

bool WindowObject::Resize(int w, int h, int x, int y)
{
    if (fsMode != Video::Windowed)
        return false;

    RECT rc = {0, 0, w, h};
    AdjustWindowRect(&rc, dwStyle, FALSE);

    // If X/Y == CW_USEDEFAULT, get current coords because Win32 is a bitch.
    WINDOWPLACEMENT wp;
    GetWindowPlacement(this->hWnd, &wp);
    if(x == CW_USEDEFAULT) x = wp.rcNormalPosition.left;
    if(y == CW_USEDEFAULT) y = wp.rcNormalPosition.top;

    return SetWindowPos(this->hWnd, nullptr, x, y, rc.right - rc.left, rc.bottom - rc.top,
        SWP_NOZORDER | SWP_FRAMECHANGED);
}

bool WindowObject::SetWindowMode(Video::FullscreenMode mode, int w, int h, int x, int y)
{
    fsMode = mode;
    switch (mode)
    {
    case Video::Windowed:
    {
        RECT rc = {0, 0, w, h};
        AdjustWindowRect(&rc, dwStyle, FALSE);
        SetWindowLongPtr(this->hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        SetWindowPos(this->hWnd, nullptr, x, y, rc.right - rc.left, rc.bottom - rc.top,
                     SWP_NOZORDER | SWP_FRAMECHANGED);
        Show();
        return true;
    }
    break;
    case Video::Borderless:
    case Video::Fullscreen:
    {
        SetWindowLongPtr(this->hWnd, GWL_STYLE, WS_POPUP);
        SetWindowPos(this->hWnd, nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_FRAMECHANGED);
        Show();
        return true;
    }
    break;
    default:
        return false;
    }
}