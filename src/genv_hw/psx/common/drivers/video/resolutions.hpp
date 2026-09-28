/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * resolutions.hpp - Created on 05-08-2025
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

#include "common/services/video/vesa.hpp"

namespace PS1::GPU
{
    using namespace VESA;

    enum PS1_VideoList : uint8_t
    {
        PS1_256x240P,
        PS1_320x240P,
        PS1_368x240P,
        PS1_512x240P,
        PS1_640x240P,
        PS1_256x240I,
        PS1_320x240I,
        PS1_368x240I,
        PS1_512x240I,
        PS1_640x240I,
        PS1_256x480I,
        PS1_320x480I,
        PS1_368x480I,
        PS1_512x480I,
        PS1_640x480I
    };

    constexpr VideoResolution PS1_Resolutions[] = {
        VideoResolution("PS1 256x240p", 256, 240, AspectRatio::R16_15, CONSOLE | SDTV | PROGRESSIVE),
        VideoResolution("PS1 320x240p", 320, 240, AspectRatio::R4_3, CONSOLE | SDTV | PROGRESSIVE),
        VideoResolution("PS1 368x240p", 368, 240, AspectRatio::R4_3, CONSOLE | SDTV | PROGRESSIVE),
        VideoResolution("PS1 512x240p", 512, 240, AspectRatio::R16_9, CONSOLE | SDTV | PROGRESSIVE | WIDESCREEN),
        VideoResolution("PS1 640x240p", 640, 240, AspectRatio::R16_9, CONSOLE | SDTV | PROGRESSIVE | WIDESCREEN),
        VideoResolution("PS1 256x240i", 256, 240, AspectRatio::R16_15, CONSOLE | SDTV | INTERLACED),
        VideoResolution("PS1 320x240i", 320, 240, AspectRatio::R4_3, CONSOLE | SDTV | INTERLACED),
        VideoResolution("PS1 368x240i", 368, 240, AspectRatio::R4_3, CONSOLE | SDTV | INTERLACED),
        VideoResolution("PS1 512x240i", 512, 240, AspectRatio::R16_9, CONSOLE | SDTV | INTERLACED | WIDESCREEN),
        VideoResolution("PS1 640x240i", 640, 240, AspectRatio::R16_9, CONSOLE | SDTV | INTERLACED | WIDESCREEN),
        VideoResolution("PS1 256x480i", 256, 480, AspectRatio::R16_15, CONSOLE | SDTV | INTERLACED),
        VideoResolution("PS1 320x480i", 320, 480, AspectRatio::R4_3, CONSOLE | SDTV | INTERLACED),
        VideoResolution("PS1 368x480i", 368, 480, AspectRatio::R4_3, CONSOLE | SDTV | INTERLACED),
        VideoResolution("PS1 512x480i", 512, 480, AspectRatio::R4_3, CONSOLE | SDTV | INTERLACED),
        VideoResolution("PS1 640x480i", 640, 480, AspectRatio::R4_3, CONSOLE | SDTV | INTERLACED)};

    constexpr uint16_t PS1_Refresh_Rates[2] = {50, 60};

    constexpr auto PS1_Video_Modes = VideoModeList(PS1_Resolutions, PS1_Refresh_Rates);

} // namespace PS1::GPU