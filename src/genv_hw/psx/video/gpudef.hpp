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

namespace System::PSX::GPU {
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

    // VRAM layout
        static constexpr int MIN_TILE_SIZE = 8;   // Tile: Minimum of 8×8 pixels (GENV)
        static constexpr int PAGE_PIXELS = 256;   // 256×256-pixel pages
        static constexpr int PAGE_GRID_COLS = 16; // pages per row

        // Derived
        static constexpr int VRAM_WIDTH = PAGE_PIXELS * PAGE_GRID_COLS;      // 4096px total width in a row
        static constexpr int TILES_PER_COL = PAGE_PIXELS / MIN_TILE_SIZE;    // How many tiles per page column
        static constexpr int TILES_PER_ROW = TILES_PER_COL;                          // How many tiles per page row
        static constexpr int TILES_PER_PAGE = TILES_PER_COL * TILES_PER_COL; // How many tiles per page
        static constexpr int MAX_CLUT_LINES_PER_PAGE = MIN_TILE_SIZE * 5;            // How many lines of CLUTs to use before considering there's too many. As this eats into tile space, this is in multiples of MIN_TILE_SIZE.

        static constexpr int COLORS_4BPP = 16;
        static constexpr int COLORS_8BPP = 256;
        static constexpr int COLORS_16BPP = 65536;

        enum : uint8_t
        {
            VRAM_1MIB = 1,
            VRAM_2MIB = 2,
            // Was higher amounts even possible?
        };

        constexpr size_t bppPxWidth(const uint8_t bpp) {
            return (bpp == Textures::BPP_8BIT ? PSX::GPU::COLORS_8BPP : PSX::GPU::COLORS_4BPP);
        }
}