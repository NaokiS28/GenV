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

#include "audio.hpp"
#include "directsound.hpp"
// #include "spu-sdl.hpp"

using namespace Audio;

IAudio *Win32::CreateAudioDriver(AudioType type, WindowObject *wObject)
{
    IAudio *driver = nullptr;
    if (wObject)
    {
        switch (type)
        {
        case AD_WIN_DSOUND:
            driver = new DirectSound(wObject);
            break;
        default:
            break;
        }
    }
    return driver;
}