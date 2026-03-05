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

    // These flags denote certain attributes for the particular resolution.
    // These can help to narrow down a list for a game to pick from if multiple options exist
    enum ResolutionFlags : uint16_t
    {
        NONE        = 0x0000,
        PC          = 0x0001, // Video monitor is a PC monitor
        PC_UNCOMMON = 0x0002, // Video monitor is a PC monitor, but resolution is uncommon and might not be supported
        TV          = 0x0004, // Video monitor is a standard TV
        CONSOLE     = 0x0008, // Source device is a standard console
        PORTABLE    = 0x0010, // Source device is a portable games console
        WIDESCREEN  = 0x0080, // Resolution is widescreen
        INTERLACED  = 0x0100, // Resolution is interlaced
        PROGRESSIVE = 0x0200, // Resolution is progressive
        SDTV        = 0x0400, // Resolution is an SDTV standard
        HDTV        = 0x0800  // Resolution is an HDTV standard
    };

    constexpr ResolutionFlags operator|(ResolutionFlags l, ResolutionFlags r)
    {
        return static_cast<ResolutionFlags>(static_cast<uint16_t>(l) | static_cast<uint16_t>(r));
    }

    struct VideoResolution
    {
        const char *name      = nullptr;
        uint16_t width        = 0;
        uint16_t height       = 0;
        AspectRatio aspect    = AspectRatio::INVALID;
        ResolutionFlags flags = NONE;

        constexpr VideoResolution() {}
        constexpr VideoResolution(const char *name, uint16_t w, uint16_t h, AspectRatio ratio, ResolutionFlags flags)
            : name(name), width(w), height(h), aspect(ratio), flags(flags)
        {
        }
    };

    struct VideoModeList
    {
        const uint16_t resLength       = 0;
        const VideoResolution *resList = nullptr;
        const uint8_t refreshLength    = 0;
        const uint16_t *refreshList    = nullptr;

        constexpr VideoModeList() {}
        template <int N, int C>
        constexpr VideoModeList(const VideoResolution (&v)[N], const uint16_t (&r)[C])
            : resLength(N), resList(v),
              refreshLength(C), refreshList(r)
        {
        }
    };

    constexpr auto BlankVideoList = VideoModeList();

    constexpr auto Custom = VideoResolution("Custom", 0, 0, AspectRatio::INVALID, NONE);
    constexpr auto CGA    = VideoResolution("CGA", 320, 200, AspectRatio::R16_10, PC_UNCOMMON | PROGRESSIVE);
    constexpr auto QVGA   = VideoResolution("QVGA", 320, 240, AspectRatio::R4_3, PORTABLE | SDTV | PROGRESSIVE);
    constexpr auto FWQVGA = VideoResolution("FWQVGA", 432, 240, AspectRatio::R16_9, PORTABLE | WIDESCREEN | SDTV | PROGRESSIVE);
    constexpr auto WQVGA  = VideoResolution("WQVGA", 384, 240, AspectRatio::R16_10, PORTABLE | WIDESCREEN | SDTV | PROGRESSIVE);
    constexpr auto HVGA   = VideoResolution("HVGA", 480, 320, AspectRatio::R3_2, PORTABLE | SDTV | PROGRESSIVE);
    constexpr auto VGA    = VideoResolution("VGA", 640, 480, AspectRatio::R4_3, PC | SDTV | PROGRESSIVE);
    constexpr auto NTSC   = VideoResolution("NTSC", 640, 480, AspectRatio::R4_3, TV | CONSOLE | SDTV | INTERLACED);
    constexpr auto WVGA   = VideoResolution("WVGA", 768, 480, AspectRatio::R5_3, PC | PORTABLE | WIDESCREEN | SDTV | PROGRESSIVE);
    constexpr auto PAL    = VideoResolution("PAL", 768, 576, AspectRatio::R4_3, TV | CONSOLE | SDTV | INTERLACED);
    constexpr auto WNTSC  = VideoResolution("NTSC (Widescreen)", 854, 480, AspectRatio::R16_9, TV | CONSOLE | WIDESCREEN | SDTV | INTERLACED);
    constexpr auto SVGA   = VideoResolution("SVGA", 800, 600, AspectRatio::R4_3, PC | SDTV | PROGRESSIVE);
    constexpr auto WPAL   = VideoResolution("PAL (Widescreen)", 1024, 576, AspectRatio::R16_9, TV | CONSOLE | WIDESCREEN | SDTV | INTERLACED);
    constexpr auto XHD    = VideoResolution("HD (4:3)", 960, 720, AspectRatio::R4_3, PC_UNCOMMON | HDTV | PROGRESSIVE);
    constexpr auto HD     = VideoResolution("HD", 1280, 720, AspectRatio::R16_9, PC | TV | CONSOLE | WIDESCREEN | HDTV | PROGRESSIVE);
    constexpr auto XGA    = VideoResolution("XGA", 1024, 768, AspectRatio::R4_3, PC | SDTV | PROGRESSIVE);
    constexpr auto WXGA   = VideoResolution("WXGA", 1280, 768, AspectRatio::R16_10, PC | WIDESCREEN | HDTV | PROGRESSIVE);
    constexpr auto SXGA   = VideoResolution("SXGA", 1280, 1024, AspectRatio::R5_4, PC | HDTV | PROGRESSIVE);
    constexpr auto SXGAM  = VideoResolution("SXGA-", 1280, 960, AspectRatio::R4_3, PC_UNCOMMON | HDTV | PROGRESSIVE);
    constexpr auto FWXGA  = VideoResolution("FWXGA", 1366, 768, AspectRatio::R16_9, PC | WIDESCREEN | HDTV | PROGRESSIVE);
    constexpr auto SXGAP  = VideoResolution("SXGA+", 1400, 1050, AspectRatio::R4_3, PC_UNCOMMON | HDTV | PROGRESSIVE);
    constexpr auto UXGA   = VideoResolution("UXGA", 1600, 1200, AspectRatio::R4_3, PC_UNCOMMON | HDTV | PROGRESSIVE);
    constexpr auto WSXGA  = VideoResolution("WSXGA", 1680, 900, AspectRatio::R16_9, PC_UNCOMMON | WIDESCREEN | HDTV | PROGRESSIVE);
    constexpr auto WSXGAP = VideoResolution("WSXGA+", 1680, 1050, AspectRatio::R16_10, PC | WIDESCREEN | HDTV | PROGRESSIVE);
    constexpr auto XFHD   = VideoResolution("Full HD (4:3)", 1440, 1080, AspectRatio::R4_3, PC_UNCOMMON | HDTV | PROGRESSIVE);
    constexpr auto FHD    = VideoResolution("Full HD", 1920, 1080, AspectRatio::R16_9, PC | CONSOLE | TV | WIDESCREEN | HDTV | PROGRESSIVE);
    constexpr auto WUXGA  = VideoResolution("WUXGA", 1920, 1200, AspectRatio::R16_10, PC | WIDESCREEN | HDTV | PROGRESSIVE);
    constexpr auto WQHD   = VideoResolution("WQHD", 2560, 1440, AspectRatio::R16_9, PC_UNCOMMON | WIDESCREEN | HDTV | PROGRESSIVE);
    constexpr auto UHD    = VideoResolution("4K", 3840, 2160, AspectRatio::R16_9, PC | TV | CONSOLE | WIDESCREEN | HDTV | PROGRESSIVE);

} // namespace VESA