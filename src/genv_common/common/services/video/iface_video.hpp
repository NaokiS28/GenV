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

#include "common/objects/font.hpp"
#include "common/services/video/color.hpp"
#include "common/util/hash.hpp"
#include "video.hpp"
#include "common/util/rect.hpp"
#include "common/objects/texture.hpp"
#include "common/objects/sprite.hpp"
#include "common/objects/tile.hpp"

#define V_ERROR(code) GV_ERROR(GV_SERVICE_VIDEO, GV_CATEGORY_GENERIC, code)

namespace Video
{
    class IVideo
    {
    protected:
        Monitor _screen;
        size_t _frameCount    = 0;
        bool _useDoubleBuffer = true;

        Textures::TextureObject *defaultTexture = nullptr;

    public:
        IVideo()          = default;
        virtual ~IVideo() = default;

        virtual inline size_t getFrameCount()
        {
            return _frameCount;
        }

        virtual bool init()        = 0;
        virtual bool reset()       = 0;
        virtual bool beginRender() = 0;
        virtual bool endRender()   = 0;
        virtual void shutdown()    = 0;

        virtual bool waitingForVSync() = 0;
        virtual void doWaitForVSync() {}

        inline virtual size_t getBufferSize(size_t length) { return length; }

        typedef uint8_t *GraphicsData;
        virtual GraphicsData allocate(size_t length)
        {
            return new uint8_t[length];
        }

        virtual void disableDoubleBuffer()
        {
            _useDoubleBuffer = false;
        }

        virtual void enableDoubleBuffer()
        {
            _useDoubleBuffer = true;
        }

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

        virtual void fillScreen(Color color)
        {
            drawRect(0, 0, _screen.res.width, _screen.res.height, color);
        }

        inline uint16_t getHorizontalRes() { return _screen.res.width; }
        inline uint16_t getVerticalRes() { return _screen.res.height; }
        inline uint16_t getRefreshRate() { return _screen.refreshRate; }
        inline Ratio getAspectRatio() { return getAspectRatioParts(_screen.res.aspect); }
        inline const char *getVideoModeName() { return _screen.res.name; }
        inline const char *getMonitorName() { return _screen.screenName; }
        inline int getDPI() { return _screen.dpi; }
        virtual void getMonitorInfo(Monitor &m) const { m = _screen; }

        inline int setResolution(VideoResolution v, bool updateWindow = true)
        {
            return setResolution(v.width, v.height, updateWindow);
        }
        virtual int setResolution(int _width, int _height, bool updateWindow = true)
        {
            this->_screen.res.width  = _width;
            this->_screen.res.height = _height;
            return true;
        }

        // Returns a list of video output modes that the application can set and use
        const virtual VideoModeList *getSupportedResolutions() = 0;

        // Attempts to set the fullscreen state and returns current fullscreen state
        virtual bool setFullscreen(FullscreenMode mode, int w = 0, int h = 0)
        {
            return false;
        }

        inline bool toggleFullscreen()
        {
            return setFullscreen(this->getFullscreenMode() != Video::Windowed ? Video::Windowed : Video::Fullscreen, 800, 600);
        }

        virtual FullscreenMode getFullscreenMode()
        {
            return FullscreenMode::Fullscreen;
        }

        virtual void drawAlpha(int x, int y, int w, int h, int sx, int sy, uint8_t a) const = 0;

        // void newLayer(int x, int y, int drawWidth, int drawHeight);

        virtual void drawLine(int x1, int y1, int x2, int y2, int width, Color color)                = 0;
        virtual void drawGradientLine(int x1, int y1, int x2, int y2, int width, Color c1, Color c2) = 0;

        virtual void drawRect(int x, int y, int width, int height, Color color)                           = 0;
        virtual void drawGradientRectH(int x, int y, int w, int h, Color left, Color right)               = 0;
        virtual void drawGradientRectV(int x, int y, int w, int h, Color top, Color bottom)               = 0;
        virtual void drawGradientRectD(int x, int y, int w, int h, Color top, Color middle, Color bottom) = 0;

        virtual void drawGradientRect(int x, int y, int w, int h, GPUGradientMode m)                                         = 0;
        virtual void drawGradientRectHVar(int x, int y, int w, int h, Color left, Color right, int startPoint, int endPoint) = 0;
        virtual void drawGradientRectVVar(int x, int y, int w, int h, Color top, Color bottom, int startPoint, int endPoint) = 0;

        inline virtual int setDefaultFont(Fonts::FontObject *fObj) { return 0; }
        virtual int drawText(Fonts::FontObject *fObj, const char *str, int x, int y, int w, int h, Color color = Colors::White, uint8_t mode = TALIGN_LEFT) = 0;
        virtual int drawText(const char *str, int x, int y, int w, int h, Color color = Colors::White, uint8_t mode = TALIGN_LEFT)                          = 0;

        virtual Textures::TextureObject *createTexture(util::Hash objectID)
        {
            // Override this function if the video system requires a different texture object.
            Textures::TextureObject *tObj = new Textures::TextureObject(objectID);
            return (tObj != nullptr ? tObj : defaultTexture);
        }
        virtual Textures::TextureObject *createTexture(util::Hash objectID, const char *filePath)
        {
            // Override this function if the video system requires a different texture object.
            Textures::TextureObject *tObj = new Textures::TextureObject(objectID, filePath);
            return (tObj != nullptr ? tObj : defaultTexture);
        }
        virtual int uploadTexture(Textures::TextureObject *tObj)  = 0;
        virtual int releaseTexture(Textures::TextureObject *tObj) = 0;

        inline int drawTextureObject(Textures::TextureObject *tObj, int x, int y, int w, int h, RectUV area)
        {
            return drawTextureObject(
                tObj,
                x, y, w, h,
                area.u1, area.v1, area.u2, area.v2);
        }
        inline int drawTextureObject(
            Textures::TextureObject *tObj, RectWH rect,
            ifloat u1, ifloat v1, ifloat u2, ifloat v2)
        {
            return drawTextureObject(
                tObj,
                rect.x, rect.y, rect.w, rect.h,
                u1, v1, u2, v2);
        }
        inline int drawTextureObject(Textures::TextureObject *tObj, RectWH rect, RectUV area)
        {
            return drawTextureObject(
                tObj,
                rect.x, rect.y, rect.w, rect.h,
                area.u1, area.v1, area.u2, area.v2);
        }
        virtual int drawTextureObject(
            const Textures::TextureObject *tObj,
            int x, int y, int w, int h,
            ifloat u1, ifloat v1,
            ifloat u2, ifloat v2) = 0;

        virtual int drawTextureObject(
            const Textures::TextureObject *tObj,
            int x, int y,
            Vertex v[]) = 0;

        inline void drawSpriteObject(Sprites::SpriteObject *sObj)
        {
            Sprites::SpritePosition pos  = sObj->getPosition();
            Textures::TextureObject *tex = sObj->getTexture();
            drawSpriteObject(sObj, (int)pos.x, (int)pos.y, tex->width, tex->height);
        }
        inline void drawSpriteObject(Sprites::SpriteObject *sObj, int x, int y)
        {
            Textures::TextureObject *tex = sObj->getTexture();
            drawSpriteObject(sObj, x, y, tex->width, tex->height);
        }
        virtual void drawSpriteObject(Sprites::SpriteObject *sObj, int x, int y, int w, int h) = 0;

        inline void drawTileObject(Sprites::TileObject *tObj, int x, int y)
        {
            Textures::TextureObject *tex = tObj->getTexture();
            drawTileObject(tObj, x, y, tex->width, tex->height);
        }
        virtual void drawTileObject(Sprites::TileObject *sObj, int x, int y, int w, int h) = 0;
    };
} // namespace Video