/*
 * Originally from 573in1 - Copyright (C) 2022-2024 spicyjpeg
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

namespace VESA
{

    enum class AspectRatio : uint8_t
    {
        INVALID,
        R1_1,
        R3_2,
        R4_3,
        R5_3,
        R5_4,
        R16_9,
        R16_10,
        R16_15
    };

    struct Ratio
    {
        int num;
        int denom;
    };

    constexpr Ratio aspect_ratio_table[] = {
        {0, 0},   // INVALID
        {1, 1},   // R1_1
        {3, 2},   // R3_2
        {4, 3},   // R4_3
        {5, 3},   // R5_3
        {5, 4},   // R5_4
        {16, 9},  // R16_9
        {16, 10}, // R16_10
        {16, 15}, // R16_10
    };

    constexpr Ratio getAspectRatioParts(AspectRatio ar)
    {
        return aspect_ratio_table[static_cast<uint8_t>(ar)];
    }

    enum VideoFlags : uint16_t
    {
        NONE = 0x0000,
        PC = 0x0001,
        PC_UNCOMMON = 0x0002,
        TV = 0x0004,
        CONSOLE = 0x0008,
        PORTABLE = 0x0010,
        WIDESCREEN = 0x0080,
        INTERLACED = 0x0100,
        PROGRESSIVE = 0x0200,
        SDTV = 0x0400,
        HDTV = 0x0800
    };

    struct VideoResolution
    {
        const char *name;
        uint16_t width = 0;
        uint16_t height = 0;
        AspectRatio aspect = AspectRatio::INVALID;
        uint16_t flags = NONE;
    };

    struct VideoModeList
    {
        const uint16_t resLength = 0;
        const VideoResolution *resList = nullptr;
        const uint8_t refreshLength = 0;
        const uint16_t *refreshList = nullptr;
    };

    constexpr VideoResolution Custom = {"Custom", 0, 0, AspectRatio::INVALID, NONE};
    constexpr VideoResolution CGA = {"CGA", 320, 200, AspectRatio::R16_10, PC_UNCOMMON | PROGRESSIVE};
    constexpr VideoResolution QVGA = {"QVGA", 320, 240, AspectRatio::R4_3, PORTABLE | SDTV | PROGRESSIVE};
    constexpr VideoResolution FWQVGA = {"FWQVGA", 432, 240, AspectRatio::R16_9, PORTABLE | WIDESCREEN | SDTV | PROGRESSIVE};
    constexpr VideoResolution WQVGA = {"WQVGA", 384, 240, AspectRatio::R16_10, PORTABLE | WIDESCREEN | SDTV | PROGRESSIVE};
    constexpr VideoResolution HVGA = {"HVGA", 480, 320, AspectRatio::R3_2, PORTABLE | SDTV | PROGRESSIVE};
    constexpr VideoResolution VGA = {"VGA", 640, 480, AspectRatio::R4_3, PC | SDTV | PROGRESSIVE};
    constexpr VideoResolution NTSC = {"NTSC", 640, 480, AspectRatio::R4_3, TV | CONSOLE | SDTV | INTERLACED};
    constexpr VideoResolution WVGA = {"WVGA", 768, 480, AspectRatio::R5_3, PC | PORTABLE | WIDESCREEN | SDTV | PROGRESSIVE};
    constexpr VideoResolution PAL = {"PAL", 768, 576, AspectRatio::R4_3, TV | CONSOLE | SDTV | INTERLACED};
    constexpr VideoResolution WNTSC = {"NTSC (Widescreen)", 854, 480, AspectRatio::R16_9, TV | CONSOLE | WIDESCREEN | SDTV | INTERLACED};
    constexpr VideoResolution SVGA = {"SVGA", 800, 600, AspectRatio::R4_3, PC | SDTV | PROGRESSIVE};
    constexpr VideoResolution WPAL = {"PAL (Widescreen)", 1024, 576, AspectRatio::R16_9, TV | CONSOLE | WIDESCREEN | SDTV | INTERLACED};
    constexpr VideoResolution XHD = {"HD (4:3)", 960, 720, AspectRatio::R4_3, PC_UNCOMMON | HDTV | PROGRESSIVE};
    constexpr VideoResolution HD = {"HD", 1280, 720, AspectRatio::R16_9, PC | TV | CONSOLE | WIDESCREEN | HDTV | PROGRESSIVE};
    constexpr VideoResolution XGA = {"XGA", 1024, 768, AspectRatio::R4_3, PC | SDTV | PROGRESSIVE};
    constexpr VideoResolution WXGA = {"WXGA", 1280, 768, AspectRatio::R16_10, PC | WIDESCREEN | HDTV | PROGRESSIVE};
    constexpr VideoResolution SXGA = {"SXGA", 1280, 1024, AspectRatio::R5_4, PC | HDTV | PROGRESSIVE};
    constexpr VideoResolution SXGAM = {"SXGA-", 1280, 960, AspectRatio::R4_3, PC_UNCOMMON | HDTV | PROGRESSIVE};
    constexpr VideoResolution FWXGA = {"FWXGA", 1366, 768, AspectRatio::R16_9, PC | WIDESCREEN | HDTV | PROGRESSIVE};
    constexpr VideoResolution SXGAP = {"SXGA+", 1400, 1050, AspectRatio::R4_3, PC_UNCOMMON | HDTV | PROGRESSIVE};
    constexpr VideoResolution UXGA = {"UXGA", 1600, 1200, AspectRatio::R4_3, PC_UNCOMMON | HDTV | PROGRESSIVE};
    constexpr VideoResolution WSXGA = {"WSXGA", 1680, 900, AspectRatio::R16_9, PC_UNCOMMON | WIDESCREEN | HDTV | PROGRESSIVE};
    constexpr VideoResolution WSXGAP = {"WSXGA+", 1680, 1050, AspectRatio::R16_10, PC | WIDESCREEN | HDTV | PROGRESSIVE};
    constexpr VideoResolution XFHD = {"Full HD (4:3)", 1440, 1080, AspectRatio::R4_3, PC_UNCOMMON | HDTV | PROGRESSIVE};
    constexpr VideoResolution FHD = {"Full HD", 1920, 1080, AspectRatio::R16_9, PC | CONSOLE | TV | WIDESCREEN | HDTV | PROGRESSIVE};
    constexpr VideoResolution WUXGA = {"WUXGA", 1920, 1200, AspectRatio::R16_10, PC | WIDESCREEN | HDTV | PROGRESSIVE};
    constexpr VideoResolution WQHD = {"WQHD", 2560, 1440, AspectRatio::R16_9, PC_UNCOMMON | WIDESCREEN | HDTV | PROGRESSIVE};
    constexpr VideoResolution UHD = {"4K", 3840, 2160, AspectRatio::R16_9, PC | TV | CONSOLE | WIDESCREEN | HDTV | PROGRESSIVE};


}