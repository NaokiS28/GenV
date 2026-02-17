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

#include "common/objects/texture.hpp"
#include "common/services/video/vesa.hpp"
#include "common/services/video/video.hpp"

namespace System::PSX::GPU
{
    using namespace VESA;

    constexpr Video::VideoResolution PSX_Resolutions[] = {
        Video::VideoResolution("PSX 256x240p", 256, 240, Video::AspectRatio::R16_15, CONSOLE | SDTV | PROGRESSIVE),
        Video::VideoResolution("PSX 320x240p", 320, 240, Video::AspectRatio::R4_3, CONSOLE | SDTV | PROGRESSIVE),
        Video::VideoResolution("PSX 368x240p", 368, 240, Video::AspectRatio::R4_3, CONSOLE | SDTV | PROGRESSIVE),
        Video::VideoResolution("PSX 512x240p", 512, 240, Video::AspectRatio::R16_9, CONSOLE | SDTV | PROGRESSIVE | WIDESCREEN),
        Video::VideoResolution("PSX 640x240p", 640, 240, Video::AspectRatio::R16_9, CONSOLE | SDTV | PROGRESSIVE | WIDESCREEN),
        Video::VideoResolution("PSX 256x240i", 256, 240, Video::AspectRatio::R16_15, CONSOLE | SDTV | INTERLACED),
        Video::VideoResolution("PSX 320x240i", 320, 240, Video::AspectRatio::R4_3, CONSOLE | SDTV | INTERLACED),
        Video::VideoResolution("PSX 368x240i", 368, 240, Video::AspectRatio::R4_3, CONSOLE | SDTV | INTERLACED),
        Video::VideoResolution("PSX 512x240i", 512, 240, Video::AspectRatio::R16_9, CONSOLE | SDTV | INTERLACED | WIDESCREEN),
        Video::VideoResolution("PSX 640x240i", 640, 240, Video::AspectRatio::R16_9, CONSOLE | SDTV | INTERLACED | WIDESCREEN),
        Video::VideoResolution("PSX 256x480i", 256, 480, Video::AspectRatio::R16_15, CONSOLE | SDTV | INTERLACED),
        Video::VideoResolution("PSX 320x480i", 320, 480, Video::AspectRatio::R4_3, CONSOLE | SDTV | INTERLACED),
        Video::VideoResolution("PSX 368x480i", 368, 480, Video::AspectRatio::R4_3, CONSOLE | SDTV | INTERLACED),
        Video::VideoResolution("PSX 512x480i", 512, 480, Video::AspectRatio::R4_3, CONSOLE | SDTV | INTERLACED),
        Video::VideoResolution("PSX 640x480i", 640, 480, Video::AspectRatio::R4_3, CONSOLE | SDTV | INTERLACED)};

    constexpr uint16_t PSX_Refresh_Rates[2] = {50, 60};

    constexpr auto PSX_Video_Modes = Video::VideoModeList(PSX_Resolutions, PSX_Refresh_Rates);

} // namespace System::PSX::GPU