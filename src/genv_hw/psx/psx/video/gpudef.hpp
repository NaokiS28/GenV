/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * gpudef.h - Created on 05-08-2025
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
#include "common/services/video/video.hpp"

namespace System::PSX::GPU
{
    using namespace VESA;
    constexpr const Video::VideoResolution PSX_Resolutions[] = {
        {"PSX 256x240p", 256, 240, Video::AspectRatio::R16_15, PORTABLE | SDTV | PROGRESSIVE},
        {"PSX 320x240p", 320, 240, Video::AspectRatio::R4_3, PORTABLE | SDTV | PROGRESSIVE},
        {"PSX 368x240p", 368, 240, Video::AspectRatio::R4_3, PORTABLE | SDTV | PROGRESSIVE},
        {"PSX 512x240p", 512, 240, Video::AspectRatio::R16_9, PORTABLE | SDTV | PROGRESSIVE | WIDESCREEN},
        {"PSX 640x240p", 640, 240, Video::AspectRatio::R16_9, PORTABLE | SDTV | PROGRESSIVE | WIDESCREEN},
        {"PSX 256x240i", 256, 240, Video::AspectRatio::R16_15, PORTABLE | SDTV | INTERLACED},
        {"PSX 320x240i", 320, 240, Video::AspectRatio::R4_3, PORTABLE | SDTV | INTERLACED},
        {"PSX 368x240i", 368, 240, Video::AspectRatio::R4_3, PORTABLE | SDTV | INTERLACED},
        {"PSX 512x240i", 512, 240, Video::AspectRatio::R16_9, PORTABLE | SDTV | INTERLACED | WIDESCREEN},
        {"PSX 640x240i", 640, 240, Video::AspectRatio::R16_9, PORTABLE | SDTV | INTERLACED | WIDESCREEN},
        {"PSX 256x480i", 256, 480, Video::AspectRatio::R16_15, PORTABLE | SDTV | INTERLACED},
        {"PSX 320x480i", 320, 480, Video::AspectRatio::R4_3, PORTABLE | SDTV | INTERLACED},
        {"PSX 368x480i", 368, 480, Video::AspectRatio::R4_3, PORTABLE | SDTV | INTERLACED},
        {"PSX 512x480i", 512, 480, Video::AspectRatio::R16_9, PORTABLE | SDTV | INTERLACED},
        {"PSX 640x480i", 640, 480, Video::AspectRatio::R16_9, PORTABLE | SDTV | INTERLACED}};

    constexpr const uint16_t PSX_Refresh_Rates[2] = {50, 60};

    constexpr const Video::VideoModeList PSX_Video_Modes = {
        .resLength = (sizeof(PSX_Resolutions) / sizeof(VideoResolution)),
        .resList = PSX_Resolutions,
        .refreshLength = 2,
        .refreshList = PSX_Refresh_Rates};

    constexpr const int iPSXDMAListSize = 1024;
    constexpr const uint8_t bPSXDMAChunkSize = 16;

    enum : uint8_t
    {
        VRAM_1MIB = 1,
        VRAM_2MIB = 2,
        // Was higher amounts even possible?
    };

} // namespace System::PSX::GPU