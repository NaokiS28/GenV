/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * basevideo.cpp - Created on 18-07-2026
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

#include "basevideo.hpp"
#include "common/return_codes.hpp"

namespace Video
{

    int BaseVideoDriver::setScreenConfig(Screen &screen, ScreenConfig &config)
    {
        screen.res     = config.res;
        screen.refresh = config.refresh;
        screen.name    = config.name;
        screen.dpi     = config.dpi;
        return GV_OK;
    }

    int BaseVideoDriver::setScreenResolution(Screen &screen, VideoResolution mode)
    {
        screen.res = mode;
        return GV_OK;
    }

} // namespace Video