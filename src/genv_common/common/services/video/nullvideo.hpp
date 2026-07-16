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
#include "common/services/system/iface_videodrv.hpp"
#include "common/util/rect.hpp"
#include "common/objects/texture.hpp"
#include "common/objects/sprite.hpp"
#include "common/objects/tile.hpp"
#include "common/services/video/vesa.hpp"

namespace Video
{
    class NullVideo : public System::IVideoDriver
    {

    public:
        NullVideo();
        ~NullVideo() = default;

        int init() override;
        bool update() override;
        bool reset() override;
        bool beginRender() override;
        bool endRender() override;
        void shutdown() override;

        bool waitingForVSync() override { return false; }

        // Returns a list of video output modes that the application can set and use
        const VESA::VideoModeList *getSupportedResolutions();

        void drawAlpha(int x, int y, int w, int h, int sx, int sy, uint8_t a) const override;

        // void newLayer(int x, int y, int drawWidth, int drawHeight);

        void drawLine(int x1, int y1, int x2, int y2, int width, Color color) override;
        void drawGradientLine(int x1, int y1, int x2, int y2, int width, Color c1, Color c2) override;

        void drawRect(int x, int y, int width, int height, Color color) override;
        void drawGradientRectH(int x, int y, int w, int h, Color left, Color right) override;
        void drawGradientRectV(int x, int y, int w, int h, Color top, Color bottom) override;
        void drawGradientRectD(int x, int y, int w, int h, Color top, Color middle, Color bottom) override;

        void drawGradientRect(int x, int y, int w, int h, GPUGradientMode m) override;
        void drawGradientRectHVar(int x, int y, int w, int h, Color left, Color right, int startPoint, int endPoint) override;
        void drawGradientRectVVar(int x, int y, int w, int h, Color top, Color bottom, int startPoint, int endPoint) override;

        int drawText(const char *str, int x, int y, int w, int h, Color color = Colors::White, uint8_t mode = TALIGN_LEFT) override;
        int drawText(Fonts::FontObject *Obj, const char *str, int x, int y, int w, int h, Color color = Colors::White, uint8_t mode = TALIGN_LEFT) override
        {
            return drawText(str, x, y, w, h, color, mode);
        }

        int uploadTexture(Textures::TextureObject *tObj) override;
        int releaseTexture(Textures::TextureObject *tObj) override;

        void drawSpriteObject(Sprites::SpriteObject *sObj, int x, int y, int w, int h) override;
        void drawTileObject(Sprites::TileObject *sObj, int x, int y, int w, int h) override;

        virtual int drawTextureObject(
            const Textures::TextureObject *tObj,
            int x, int y, int w, int h,
            ifloat u1, ifloat v1,
            ifloat u2, ifloat v2) override;

        virtual int drawTextureObject(
            const Textures::TextureObject *tObj,
            int x, int y,
            Vertex v[]) override;
    };
} // namespace Video