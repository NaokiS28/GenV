/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * gpu.hpp - Created on 27-04-2025
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
#include <stddef.h>

#include "vesa.hpp"
#include "color.hpp"

namespace Video
{
    using namespace VESA;

    enum FullscreenMode : uint8_t
    {
        Windowed,   // App is in window mode
        Borderless, // App is in borderless fullscreen mode
        Fullscreen  // App is in dedicated, resolution switching fullscreen mode
    };

    enum VideoResult : int
    {
        V_RES_UNSUPPORTED = -3,
        V_RES_LIST_INVALID = -2,
        V_RES_INVALID = -1,
        V_OK,
        V_RES_TOO_HIGH,                   // The requested resolution was beyond the system's capabilities, the resolution was clamped
        V_RES_MODIFIED = 128,             // The requested resolution didn't exist in the video drivers capabilities, was modified to the nearest fit.
        V_REFRESH_MODIFIED = (0x80 << 8), // The requested refresh rate didn't exist in the video drivers capabilities, was modified to the nearest fit.
    };

    // DPI/Scaling stuff

    enum MonitorDPI : uint8_t
    {
        DPI_96 = 96,
        DPI_120 = 120,
        DPI_144 = 144,
        DPI_168 = 168,
        DPI_192 = 192,
        DPI_216 = 216,
        DPI_240 = 240
    };

    struct DPIScale
    {
        int real_min;
        int real_max;
        int logical_dpi;
        int scale; // Percent
    };

    constexpr DPIScale dpi_table[] = {
        {0, 109, DPI_96, 100},
        {110, 130, DPI_120, 125},
        {131, 150, DPI_144, 150},
        {151, 180, DPI_168, 175},
        {181, 210, DPI_192, 200},
        {211, 240, DPI_216, 225},
        {241, 10000, DPI_240, 250}};

    constexpr DPIScale getNearestScale(int real_dpi)
    {
        for (const auto &entry : dpi_table)
        {
            if (real_dpi >= entry.real_min && real_dpi <= entry.real_max)
                return entry;
        }
        return dpi_table[DPI_96]; // fallback to 96 DPI
    }

    struct Monitor
    {
        VideoResolution res;
        int16_t refreshRate, dpi = DPI_96;
        const char *screenName = nullptr;
        uint8_t monitorNumber = 0;
    };

    enum GPUGradientMode : uint32_t
    {
        GPU_GRADIENT_H = 0,
        GPU_GRADIENT_V = 1,
        GPU_GRADIENT_Ds
    };

    enum TextAlign
    {
        TALIGN_LEFT,
        TALIGN_CENTER,
        TALIGN_RIGHT
    };

    uint32_t findNearestVideoMode(const VideoModeList *list, uint16_t w, uint16_t h, uint16_t r = 60);
    size_t msToFrames(size_t millis);
    size_t frames();
    uint16_t getHorizontalRes();
    uint16_t getVerticalRes();
}