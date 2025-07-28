/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * video.hpp - Created on 26-04-2025
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
#include "common/services/video/video.hpp"
#include "window-mgr.hpp"

using namespace Video;

class IWinVideo : public IVideo
{
public:
    virtual bool setWindow(WindowObject *wObj) = 0;
};

namespace Win32
{
    enum VideoType : uint8_t
    {
        VD_WIN_D3D,
        VD_WIN_OPENGL
    };

    IWinVideo *CreateVideoDriver(VideoType type, WindowObject *wObject);
}