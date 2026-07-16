/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * screen.hpp - Created on 15-07-2026
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
#include "common/services/system/iface_videodrv.hpp"

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

    /*
     * Screen Class
     * ==============
     * Screens are literal render contexts used to denote either a video monitor or
     * a video output window that games can render to. They serve as the interposer
     * between the GPU driver and the abstracted game as the game will run functions
     * to modify a Screen, the screen then asks the related GPU device to serve that
     * request.
     */
    class Screen
    {
    private:
        System::IVideoDriver *const _gpu;

        VideoResolution res = VideoResolution();
        uint16_t refresh    = 60;
        uint16_t dpi        = DPI_96;
        const char *name    = nullptr;
        uint8_t number      = 0;

    public:
        constexpr Screen(
            System::IVideoDriver *const gpu,
            VideoResolution res,
            uint16_t refresh,
            uint16_t dpi,
            const char *const name,
            uint8_t number)
            : _gpu(gpu),
              res(res),
              dpi(dpi),
              name(name),
              number(number)
        {
        }

        inline uint16_t getHorizontalRes() { return res.width; }
        inline uint16_t getVerticalRes() { return res.height; }
        inline uint16_t getRefreshRate() { return refresh; }
        inline Ratio getAspectRatio() { return getAspectRatioParts(res.aspect); }
        inline const char *getVideoModeName() { return res.name; }
        inline const char *getName() { return name; }
        inline int getDPI() { return dpi; }

        //!Review
        // The associated GPU driver for this screen. Screen does NOT own it: the
        // System class owns the driver's lifecycle. This pointer is a convenience
        // handle so draw forwarders reach the driver without per-call indirection.
        inline System::IVideoDriver *getDriver() const { return _gpu; }
        inline uint8_t getNumber() const { return number; }
        //!End

        inline int setResolution(VESA::VideoResolution v, bool updateWindow = true)
        {
            return setResolution(v.width, v.height, updateWindow);
        }
        int setResolution(int _width, int _height, bool updateWindow = true)
        {
            this->res.width  = _width;
            this->res.height = _height;
            return true;
        }

        // Attempts to set the fullscreen state and returns current fullscreen state
        bool setFullscreen(FullscreenMode mode, int w = 0, int h = 0)
        {
            return false;
        }

        inline bool toggleFullscreen()
        {
            return setFullscreen(this->getFullscreenMode() != Video::Windowed ? Video::Windowed : Video::Fullscreen, 800, 600);
        }

        FullscreenMode getFullscreenMode()
        {
            return FullscreenMode::Fullscreen;
        }

        void fillScreen(Color color)
        {
            _gpu->drawRect(0, 0, res.width, res.height, color);
        }

        // Returns a list of video output modes that the application can set and use
        const virtual VideoModeList *getSupportedResolutions();
    };
} // namespace Video