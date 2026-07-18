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

#include "common/services/adminkey.hpp"
#include "vesa.hpp"
#include "common/services/system/iface_videodrv.hpp"

namespace Video
{
    using namespace VESA;

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

    //! Review
    // Config a video driver hands System::assignScreen when registering a screen.
    // name == nullptr -> the allocator stamps the slot's DISPLAY default (displays.hpp).
    struct ScreenConfig
    {
        VideoResolution res;
        uint16_t refresh = 60;
        uint16_t dpi     = DPI_96;
        const char *name = nullptr;
    };
    //! End

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
        friend class BaseVideoDriver;

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

        // The associated GPU driver for this screen. Screen does NOT own it: the
        // System class owns the driver's lifecycle. This pointer is a convenience
        // handle so draw forwarders reach the driver without per-call indirection.
        inline System::IVideoDriver *getDriver() const { return _gpu; }
        inline uint8_t getNumber() const { return number; }

        void fillScreen(Color color)
        {
            _gpu->drawRect(*this, 0, 0, res.width, res.height, color);
        }

        // Non-draw forwarders.
        //
        inline int setResolution(VESA::VideoResolution v, bool updateWindow = true)
        {
            return setResolution(v.width, v.height, updateWindow);
        }
        inline int setResolution(int _width, int _height, bool updateWindow = true)
        {
            return _gpu->setResolution(*this, _width, _height);
        }

        // Attempts to set the fullscreen state and returns current fullscreen state
        // TODO: Should this be a system call? Logically it's a screen function, but realistically all GPUs should go full screen
        bool setFullscreen(System::FullscreenMode mode, int w = 0, int h = 0)
        {
            return false;
        }

        inline bool toggleFullscreen()
        {
            return setFullscreen(this->getFullscreenMode() != System::Windowed ? System::Windowed : System::Fullscreen, 800, 600);
        }

        System::FullscreenMode getFullscreenMode()
        {
            return System::FullscreenMode::Fullscreen;
        }

        // Draw forwarders. A game draws by calling these on the Screen it holds; the
        // Screen hands its own *this to the driver as the target Screen&, so the game
        // never has to name a driver. Output only - resource ops (createTexture etc.)
        // are driver-scoped and reached through getDriver().
        inline bool beginRender() { return _gpu->beginRender(*this); }
        inline bool endRender() { return _gpu->endRender(*this); }

        inline void drawAlpha(int x, int y, int w, int h, int sx, int sy, uint8_t a) { _gpu->drawAlpha(*this, x, y, w, h, sx, sy, a); }

        inline void drawLine(int x1, int y1, int x2, int y2, int width, Color color) { _gpu->drawLine(*this, x1, y1, x2, y2, width, color); }
        inline void drawGradientLine(int x1, int y1, int x2, int y2, int width, Color c1, Color c2) { _gpu->drawGradientLine(*this, x1, y1, x2, y2, width, c1, c2); }

        inline void drawRect(int x, int y, int width, int height, Color color) { _gpu->drawRect(*this, x, y, width, height, color); }
        inline void drawRect(RectWH rect, Color color) { _gpu->drawRect(*this, rect.x, rect.y, rect.w, rect.h, color); }

        inline void drawGradientRectH(int x, int y, int w, int h, Color left, Color right) { _gpu->drawGradientRectH(*this, x, y, w, h, left, right); }
        inline void drawGradientRectH(RectWH rect, Color left, Color right) { _gpu->drawGradientRectH(*this, rect.x, rect.y, rect.w, rect.h, left, right); }
        inline void drawGradientRectV(int x, int y, int w, int h, Color top, Color bottom) { _gpu->drawGradientRectV(*this, x, y, w, h, top, bottom); }
        inline void drawGradientRectV(RectWH rect, Color top, Color bottom) { _gpu->drawGradientRectV(*this, rect.x, rect.y, rect.w, rect.h, top, bottom); }
        inline void drawGradientRectD(int x, int y, int w, int h, Color top, Color middle, Color bottom) { _gpu->drawGradientRectD(*this, x, y, w, h, top, middle, bottom); }
        inline void drawGradientRectD(RectWH rect, Color top, Color middle, Color bottom) { _gpu->drawGradientRectD(*this, rect.x, rect.y, rect.w, rect.h, top, middle, bottom); }

        inline void drawGradientRect(int x, int y, int w, int h, GPUGradientMode m) { _gpu->drawGradientRect(*this, x, y, w, h, m); }
        inline void drawGradientRectHVar(int x, int y, int w, int h, Color left, Color right, int startPoint, int endPoint) { _gpu->drawGradientRectHVar(*this, x, y, w, h, left, right, startPoint, endPoint); }
        inline void drawGradientRectVVar(int x, int y, int w, int h, Color top, Color bottom, int startPoint, int endPoint) { _gpu->drawGradientRectVVar(*this, x, y, w, h, top, bottom, startPoint, endPoint); }

        inline int drawText(Fonts::FontObject *fObj, const char *str, int x, int y, int w, int h, Color color = Colors::White, uint8_t mode = TALIGN_LEFT) { return _gpu->drawText(*this, fObj, str, x, y, w, h, color, mode); }
        inline int drawText(Fonts::FontObject *fObj, const char *str, RectWH box, Color color = Colors::White, uint8_t mode = TALIGN_LEFT) { return _gpu->drawText(*this, fObj, str, box, color, mode); }
        inline int drawText(const char *str, int x, int y, int w, int h, Color color = Colors::White, uint8_t mode = TALIGN_LEFT) { return _gpu->drawText(*this, str, x, y, w, h, color, mode); }
        inline int drawText(const char *str, RectWH box, Color color = Colors::White, uint8_t mode = TALIGN_LEFT) { return _gpu->drawText(*this, str, box, color, mode); }

        inline int drawChar(const char c, int x, int y, Color color = Colors::White) { return _gpu->drawChar(*this, c, x, y, color); }
        inline int drawChar(Fonts::FontObject *fObj, const char c, int x, int y, Color color = Colors::White) { return _gpu->drawChar(*this, fObj, c, x, y, color); }

        inline int drawTextureObject(Textures::TextureObject *tObj, int x, int y, int w, int h, RectUV area) { return _gpu->drawTextureObject(*this, tObj, x, y, w, h, area); }
        inline int drawTextureObject(Textures::TextureObject *tObj, RectWH rect, ifloat u1, ifloat v1, ifloat u2, ifloat v2) { return _gpu->drawTextureObject(*this, tObj, rect, u1, v1, u2, v2); }
        inline int drawTextureObject(Textures::TextureObject *tObj, RectWH rect, RectUV area) { return _gpu->drawTextureObject(*this, tObj, rect, area); }
        inline int drawTextureObject(const Textures::TextureObject *tObj, int x, int y, int w, int h, ifloat u1, ifloat v1, ifloat u2, ifloat v2) { return _gpu->drawTextureObject(*this, tObj, x, y, w, h, u1, v1, u2, v2); }
        inline int drawTextureObject(const Textures::TextureObject *tObj, int x, int y, Vertex v[]) { return _gpu->drawTextureObject(*this, tObj, x, y, v); }

        inline void drawSpriteObject(Sprites::SpriteObject *sObj) { _gpu->drawSpriteObject(*this, sObj); }
        inline void drawSpriteObject(Sprites::SpriteObject *sObj, int x, int y) { _gpu->drawSpriteObject(*this, sObj, x, y); }
        inline void drawSpriteObject(Sprites::SpriteObject *sObj, int x, int y, int w, int h) { _gpu->drawSpriteObject(*this, sObj, x, y, w, h); }

        inline void drawTileObject(Sprites::TileObject *tObj, int x, int y) { _gpu->drawTileObject(*this, tObj, x, y); }
        inline void drawTileObject(Sprites::TileObject *sObj, int x, int y, int w, int h) { _gpu->drawTileObject(*this, sObj, x, y, w, h); }

        // Returns a list of video output modes that the application can set and use
        //! Review
        // Inline body so Screen has no out-of-line key function (Screen has no .cpp);
        // without it the vtable is never emitted and constructing a Screen fails to
        // link. Resolution enumeration is not wired yet, so this returns null for now.
        const virtual VideoModeList *getSupportedResolutions() { return nullptr; }
        //! End

        // Inline for ISystem classes to set the screen name
        // Only the GPU can set parameters arbitrarily. System can only set name outside
        // of creation.
        inline void setName(AdminClass_Key key, const char *name)
        {
            (void)key;
            this->name = name;
        }
    };
} // namespace Video