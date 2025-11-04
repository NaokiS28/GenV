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
#include "iface_video.hpp"
#include "common/util/templates.hpp"
#include "common/util/rect.h"
#include "common/objects/texture.hpp"
#include "common/objects/sprite.hpp"
#include "common/objects/tile.hpp"
#include "common/logger/log.hpp"

namespace Video
{
    class NullVideo : public IVideo
    {

    public:
        NullVideo();
        ~NullVideo() = default;

        bool init();
        bool reset();
        bool beginRender();
        bool endRender();
        bool shutdown();

        // Returns a list of video output modes that the application can set and use
        const VideoModeList *getSupportedResolutions();

        void drawAlpha(int x, int y, int w, int h, int sx, int sy, uint8_t a) const;

        // void newLayer(int x, int y, int drawWidth, int drawHeight);

        void drawLine(int x1, int y1, int x2, int y2, int width, Color color);
        void drawGradientLine(int x1, int y1, int x2, int y2, int width, Color c1, Color c2);

        void drawRect(int x, int y, int width, int height, Color color);
        void drawGradientRectH(int x, int y, int w, int h, Color left, Color right);
        void drawGradientRectV(int x, int y, int w, int h, Color top, Color bottom);
        void drawGradientRectD(int x, int y, int w, int h, Color top, Color middle, Color bottom);

        void drawGradientRect(int x, int y, int w, int h, GPUGradientMode m);
        void drawGradientRectHVar(int x, int y, int w, int h, Color left, Color right, int startPoint, int endPoint);
        void drawGradientRectVVar(int x, int y, int w, int h, Color top, Color bottom, int startPoint, int endPoint);

        int drawText(const char *str, int x, int y, int w, int h, Color color = Colors::White, uint8_t mode = TALIGN_LEFT);
        int drawText(Fonts::FontObject *Obj, const char *str, int x, int y, int w, int h, Color color = Colors::White, uint8_t mode = TALIGN_LEFT)
        {
            return drawText(str, x, y, w, h, color, mode);
        }

        int uploadTexture(Textures::TextureObject *tObj);
        int releaseTexture(Textures::TextureObject *tObj);

        void drawSpriteObject(Sprites::SpriteObject *sObj, int x, int y, int w, int h);
        void drawTileObject(Sprites::TileObject *sObj, int x, int y, int w, int h);

        virtual int drawTextureObject(
            const Textures::TextureObject *tObj,
            int x, int y, int w, int h,
            ifloat u1, ifloat v1,
            ifloat u2, ifloat v2);

        virtual int drawTextureObject(
            const Textures::TextureObject *tObj,
            int x, int y,
            Vertex v[]);
    };
} // namespace Video