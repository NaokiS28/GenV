/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * sound.hpp - Created on 25-05-2025
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

#include "common/services/services.hpp"
#include "../window-mgr.hpp"

using namespace Audio;

namespace Win32
{
    enum AudioType : uint8_t
    {
        AD_WIN_DSOUND,
        AD_WIN_WASAPI
    };

    IAudio *CreateAudioDriver(AudioType type, WindowObject *wObject);
}