/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * basevideo.hpp - Created on 18-07-2026
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
#include <stdint.h>

#include "common/services/system/iface_videodrv.hpp"
#include "common/services/video/screen.hpp"
#include "common/services/video/vesa.hpp"

namespace Video
{
    class BaseVideoDriver : public System::IVideoDriver
    {
    protected:
        int setScreenConfig(Screen &screen, ScreenConfig &config);
        int setScreenResolution(Screen &screen, VideoResolution mode);

    public:
        BaseVideoDriver(System::ISystem &sys) : System::IVideoDriver(sys) {}
        ~BaseVideoDriver() = default;
    };
} // namespace Video