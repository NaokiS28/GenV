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

#include "common/objects/font.hpp"
#include "vesa.hpp"

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
        V_RES_UNSUPPORTED  = -3,
        V_RES_LIST_INVALID = -2,
        V_RES_INVALID      = -1,
        V_OK,
        V_RES_TOO_HIGH,                   // The requested resolution was beyond the system's capabilities, the resolution was clamped
        V_RES_MODIFIED     = 128,         // The requested resolution didn't exist in the video drivers capabilities, was modified to the nearest fit.
        V_REFRESH_MODIFIED = (0x80 << 8), // The requested refresh rate didn't exist in the video drivers capabilities, was modified to the nearest fit.
    };

    // DPI/Scaling stuff

    enum MonitorDPI : uint8_t
    {
        DPI_96  = 96,
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
        {0, 109, DPI_96, 100},       // DPI: 96 pixels per inch
        {110, 130, DPI_120, 125},    // DPI: 120 pixels per inch
        {131, 150, DPI_144, 150},    // DPI: 144 pixels per inch (HiDPI)
        {151, 180, DPI_168, 175},    // DPI: 168 pixels per inch (HiDPI)
        {181, 210, DPI_192, 200},    // DPI: 192 pixels per inch (HiDPI)
        {211, 240, DPI_216, 225},    // DPI: 216 pixels per inch (HiDPI)
        {241, 10000, DPI_240, 250}}; // DPI: 240 pixels per inch (HiDPI)

    constexpr DPIScale getNearestScale(int real_dpi)
    {
        for (const auto &entry : dpi_table)
        {
            if (real_dpi >= entry.real_min && real_dpi <= entry.real_max)
                return entry;
        }
        return dpi_table[0]; // fallback to 96 DPI
    }

    struct Monitor
    {
        VideoResolution res = VideoResolution();
        int16_t refreshRate, dpi = DPI_96;
        const char *screenName = nullptr;
        uint8_t monitorNumber  = 0;
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

    size_t msToFrames(size_t millis); // Returns how many frames should elapse within a given time
    size_t frames();                  // Returns the amount of frames that have been rendered
    uint16_t getHorizontalRes();      // Returns the current horizontal screen resolution
    uint16_t getVerticalRes();        // Returns the current vertical screen resolution
    uint16_t getRefreshRate();        // Returns the current screen refresh rate

    inline int getRGB565Length(uint16_t w, uint16_t h)
    {
        return ((sizeof(uint16_t) * w) * h);
    }

    inline int getRGB888Length(uint16_t w, uint16_t h)
    {
        return (((sizeof(uint32_t) - 1) * w) * h);
    }

    inline int getARGBLength(uint16_t w, uint16_t h)
    {
        return ((sizeof(uint32_t) * w) * h);
    }

    // Runtime-friendly (assumes tightly packed)
    int toRGB565(void *dest, const void *src, size_t pixelCount);
    int toRGB888(void *dest, const void *src, size_t pixelCount);

    // Toolchain-friendly (handles stride/padding)
    int toRGB565_stride(
        void *dest, size_t destStride,
        const void *src, size_t srcStride,
        size_t width, size_t height);

    int toRGB888_stride(
        void *dest, size_t destStride,
        const void *src, size_t srcStride,
        size_t width, size_t height);

    size_t getStringWidth(Fonts::FontObject *fObj, const char *str);

} // namespace Video