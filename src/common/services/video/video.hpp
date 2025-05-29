/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * gpu.hpp - Created on 27-04-2025
 *
 * DXUX is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * DXUX is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * DXUX. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <stdint.h>
#include "common/util/templates.hpp"
#include "common/util/rect.h"

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

    class IVideo
    {
    protected:
        Monitor screen;

    public:
        IVideo() = default;
        virtual ~IVideo() = default;

        virtual bool init() = 0;
        virtual bool reset() = 0;
        virtual bool beginRender() = 0;
        virtual bool endRender() = 0;
        virtual bool shutdown() = 0;

        inline void drawRect(RectWH &rect, Color color)
        {
            drawRect(rect.x, rect.y, rect.w, rect.h, color);
        }
        inline void drawGradientRectH(RectWH &rect, Color left, Color right)
        {
            drawGradientRectH(rect.x, rect.y, rect.w, rect.h, left, right);
        }
        inline void drawGradientRectV(RectWH &rect, Color top, Color bottom)
        {
            drawGradientRectV(rect.x, rect.y, rect.w, rect.h, top, bottom);
        }
        inline void drawGradientRectD(RectWH &rect, Color top, Color middle, Color bottom)
        {
            drawGradientRectD(rect.x, rect.y, rect.w, rect.h, top, middle, bottom);
        }

        inline void drawGradientRectHVar(RectWH &rect, Color left, Color right, int startPoint, int endPoint)
        {
            drawGradientRectHVar(rect.x, rect.y, rect.w, rect.h, left, right, startPoint, endPoint);
        }
        inline void drawGradientRectVVar(RectWH &rect, Color top, Color bottom, int startPoint, int endPoint)
        {
            drawGradientRectVVar(rect.x, rect.y, rect.w, rect.h, top, bottom, startPoint, endPoint);
        }

        inline void fillScreen(Color color)
        {
            drawRect(0, 0, screen.res.width, screen.res.height, color);
        }

        inline int getHorizontalRes() { return screen.res.width; }
        inline int getVerticalRes() { return screen.res.height; }
        inline int getRefreshRate() { return screen.refreshRate; }
        inline Ratio getAspectRatio() { return getAspectRatioParts(screen.res.aspect); }
        inline const char *getVideoModeName() { return screen.res.name; }
        inline const char *getMonitorName() { return screen.screenName; }
        inline int getDPI() { return screen.dpi; }
        virtual void getMonitorInfo(Monitor &m) const { m = screen; }

        virtual int setResolution(int _width, int _height, bool updateWindow = true)
        {
            this->screen.res.width = _width;
            this->screen.res.height = _height;
            return true;
        }

        // Returns a list of video output modes that the application can set and use
        virtual int getSupportedResolutions(VideoModeList &list) = 0;

        // Attempts to set the fullscreen state and returns current fullscreen state
        virtual bool setFullscreen(FullscreenMode mode, int w = 0, int h = 0)
        {
            return false;
        }

        inline bool toggleFullscreen(){
            return setFullscreen(this->getFullscreenMode() != Video::Windowed ? Video::Windowed : Video::Fullscreen, 800, 600);
        }

        virtual FullscreenMode getFullscreenMode(){ 
            return FullscreenMode::Fullscreen; 
        }


        virtual void drawAlpha(int x, int y, int w, int h, int sx, int sy, uint8_t a) const = 0;

        // void newLayer(int x, int y, int drawWidth, int drawHeight);

        virtual void drawLine(int x1, int y1, int x2, int y2, int width, Color color) = 0;

        virtual void drawRect(int x, int y, int width, int height, Color color) = 0;
        virtual void drawGradientRectH(int x, int y, int w, int h, Color left, Color right) = 0;
        virtual void drawGradientRectV(int x, int y, int w, int h, Color top, Color bottom) = 0;
        virtual void drawGradientRectD(int x, int y, int w, int h, Color top, Color middle, Color bottom) = 0;

        virtual void drawGradientRect(int x, int y, int w, int h, GPUGradientMode m) = 0;
        virtual void drawGradientRectHVar(int x, int y, int w, int h, Color left, Color right, int startPoint, int endPoint) = 0;
        virtual void drawGradientRectVVar(int x, int y, int w, int h, Color top, Color bottom, int startPoint, int endPoint) = 0;

        virtual void drawText(const char *str, int len, int x, int y, int w, int h, Color color) = 0;
    };

    class Image
    {
    public:
        bool valid = false;
        char *name;
        uint16_t u, v, width, height;

        inline Image(void)
            : width(0), height(0) {}

        void drawScaled(int x, int y, int w, int h, bool blend = false) const;
        void draw(int x, int y, int alpha = 255) const;
        void draw(int x, int y, int sx, int sy, int w, int h, int a = 255) const;
    };
}