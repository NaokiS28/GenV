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
#include "iface_driver.hpp"
#include "common/util/hash.hpp"
#include "common/util/rect.hpp"
#include "common/objects/texture.hpp"
#include "common/objects/sprite.hpp"
#include "common/objects/tile.hpp"

#include "common/services/video/video.hpp"
#include "common/services/video/color.hpp"
#include "common/services/video/text.hpp"

#define V_ERROR(code) GV_ERROR(GV_SERVICE_VIDEO, GV_CATEGORY_GENERIC, code)

// Forward declaration only. screen.hpp includes this header, so this header must
// not include screen.hpp (circular). Output methods take a Screen by reference so
// a single driver can serve multiple screens (SGMS); resource methods are
// driver-scoped and take no Screen.
namespace Video
{
    class Screen;
}

namespace System
{
    using namespace Video;

    class IVideoDriver : public System::IDriver
    {
    protected:
        size_t _frameCount    = 0;
        bool _useDoubleBuffer = true;

        Textures::TextureObject *defaultTexture = nullptr;

    public:
        IVideoDriver()          = default;
        virtual ~IVideoDriver() = default;

        virtual inline size_t getFrameCount()
        {
            return _frameCount;
        }

        virtual bool beginRender(Video::Screen &screen) = 0;
        virtual bool endRender(Video::Screen &screen)   = 0;

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

        inline void drawRect(Video::Screen &screen, RectWH &rect, Color color)
        {
            drawRect(screen, rect.x, rect.y, rect.w, rect.h, color);
        }
        inline void drawGradientRectH(Video::Screen &screen, RectWH &rect, Color left, Color right)
        {
            drawGradientRectH(screen, rect.x, rect.y, rect.w, rect.h, left, right);
        }
        inline void drawGradientRectV(Video::Screen &screen, RectWH &rect, Color top, Color bottom)
        {
            drawGradientRectV(screen, rect.x, rect.y, rect.w, rect.h, top, bottom);
        }
        inline void drawGradientRectD(Video::Screen &screen, RectWH &rect, Color top, Color middle, Color bottom)
        {
            drawGradientRectD(screen, rect.x, rect.y, rect.w, rect.h, top, middle, bottom);
        }

        inline void drawGradientRectHVar(Video::Screen &screen, RectWH &rect, Color left, Color right, int startPoint, int endPoint)
        {
            drawGradientRectHVar(screen, rect.x, rect.y, rect.w, rect.h, left, right, startPoint, endPoint);
        }
        inline void drawGradientRectVVar(Video::Screen &screen, RectWH &rect, Color top, Color bottom, int startPoint, int endPoint)
        {
            drawGradientRectVVar(screen, rect.x, rect.y, rect.w, rect.h, top, bottom, startPoint, endPoint);
        }

        virtual void drawAlpha(Video::Screen &screen, int x, int y, int w, int h, int sx, int sy, uint8_t a) const = 0;

        // void newLayer(int x, int y, int drawWidth, int drawHeight);

        virtual void drawLine(Video::Screen &screen, int x1, int y1, int x2, int y2, int width, Color color)                = 0;
        virtual void drawGradientLine(Video::Screen &screen, int x1, int y1, int x2, int y2, int width, Color c1, Color c2) = 0;

        virtual void drawRect(Video::Screen &screen, int x, int y, int width, int height, Color color)                           = 0;
        virtual void drawGradientRectH(Video::Screen &screen, int x, int y, int w, int h, Color left, Color right)               = 0;
        virtual void drawGradientRectV(Video::Screen &screen, int x, int y, int w, int h, Color top, Color bottom)               = 0;
        virtual void drawGradientRectD(Video::Screen &screen, int x, int y, int w, int h, Color top, Color middle, Color bottom) = 0;

        virtual void drawGradientRect(Video::Screen &screen, int x, int y, int w, int h, GPUGradientMode m)                                         = 0;
        virtual void drawGradientRectHVar(Video::Screen &screen, int x, int y, int w, int h, Color left, Color right, int startPoint, int endPoint) = 0;
        virtual void drawGradientRectVVar(Video::Screen &screen, int x, int y, int w, int h, Color top, Color bottom, int startPoint, int endPoint) = 0;

        inline virtual int setDefaultFont(Fonts::FontObject *fObj) { return 0; }
        virtual int drawText(Video::Screen &screen, Fonts::FontObject *fObj, const char *str, int x, int y, int w, int h, Color color = Colors::White, uint8_t mode = TALIGN_LEFT) = 0;
        inline virtual int drawText(Video::Screen &screen, Fonts::FontObject *fObj, const char *str, RectWH box, Color color = Colors::White, uint8_t mode = TALIGN_LEFT)
        {
            return drawText(screen, fObj, str, box.x, box.y, box.w, box.h, color, mode);
        }

        virtual int drawText(Video::Screen &screen, const char *str, int x, int y, int w, int h, Color color = Colors::White, uint8_t mode = TALIGN_LEFT) = 0;
        inline virtual int drawText(Video::Screen &screen, const char *str, RectWH box, Color color = Colors::White, uint8_t mode = TALIGN_LEFT)
        {
            return drawText(screen, str, box.x, box.y, box.w, box.h, color, mode);
        }

        inline virtual int drawChar(Video::Screen &screen, const char c, int x, int y, Color color = Colors::White)
        {
            char str[2] = {c, '\0'};
            return drawText(screen, str, x, y, 10, 20, color);
        }
        inline virtual int drawChar(Video::Screen &screen, Fonts::FontObject *fObj, const char c, int x, int y, Color color = Colors::White)
        {
            char str[2] = {c, '\0'};
            return drawText(screen, fObj, str, x, y, 10, 20, color);
        }

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

        inline int drawTextureObject(Video::Screen &screen, Textures::TextureObject *tObj, int x, int y, int w, int h, RectUV area)
        {
            return drawTextureObject(
                screen,
                tObj,
                x, y, w, h,
                area.u1, area.v1, area.u2, area.v2);
        }
        inline int drawTextureObject(
            Video::Screen &screen, Textures::TextureObject *tObj, RectWH rect,
            ifloat u1, ifloat v1, ifloat u2, ifloat v2)
        {
            return drawTextureObject(
                screen,
                tObj,
                rect.x, rect.y, rect.w, rect.h,
                u1, v1, u2, v2);
        }
        inline int drawTextureObject(Video::Screen &screen, Textures::TextureObject *tObj, RectWH rect, RectUV area)
        {
            return drawTextureObject(
                screen,
                tObj,
                rect.x, rect.y, rect.w, rect.h,
                area.u1, area.v1, area.u2, area.v2);
        }
        virtual int drawTextureObject(
            Video::Screen &screen,
            const Textures::TextureObject *tObj,
            int x, int y, int w, int h,
            ifloat u1, ifloat v1,
            ifloat u2, ifloat v2) = 0;

        virtual int drawTextureObject(
            Video::Screen &screen,
            const Textures::TextureObject *tObj,
            int x, int y,
            Vertex v[]) = 0;

        inline void drawSpriteObject(Video::Screen &screen, Sprites::SpriteObject *sObj)
        {
            Sprites::SpritePosition pos  = sObj->getPosition();
            Textures::TextureObject *tex = sObj->getTexture();
            drawSpriteObject(screen, sObj, (int)pos.x, (int)pos.y, tex->width, tex->height);
        }
        inline void drawSpriteObject(Video::Screen &screen, Sprites::SpriteObject *sObj, int x, int y)
        {
            Textures::TextureObject *tex = sObj->getTexture();
            drawSpriteObject(screen, sObj, x, y, tex->width, tex->height);
        }
        virtual void drawSpriteObject(Video::Screen &screen, Sprites::SpriteObject *sObj, int x, int y, int w, int h) = 0;

        inline void drawTileObject(Video::Screen &screen, Sprites::TileObject *tObj, int x, int y)
        {
            Textures::TextureObject *tex = tObj->getTexture();
            drawTileObject(screen, tObj, x, y, tex->width, tex->height);
        }
        virtual void drawTileObject(Video::Screen &screen, Sprites::TileObject *sObj, int x, int y, int w, int h) = 0;
    };
} // namespace System
