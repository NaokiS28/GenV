/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * video.cpp - Created on 26-04-2025
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

#include "video.hpp"
#include "gpu-d3d.hpp"
#include "gpu-sdl.hpp"

IWinVideo *Win32::CreateVideoDriver(VideoType type, WindowObject *wObject)
{
    IWinVideo *driver = nullptr;
    if (wObject)
    {
        switch (type)
        {
        case VD_WIN_D3D:
            driver = new DirectXGPU(wObject);
            break;
        case VD_WIN_OPENGL:
            driver = new SDLGPU(wObject);
            break;
        }
    }
    return driver;
}