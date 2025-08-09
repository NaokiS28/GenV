/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * nullvideo.cpp - Created on 09-08-2025
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

#include "nullvideo.hpp"
namespace Video
{

    NullVideo::NullVideo() : IVideo()
    {
    }

    bool NullVideo::init() { return true; }
    bool NullVideo::reset() { return true; }
    bool NullVideo::beginRender() { return true; }
    bool NullVideo::endRender() { return true; }
    bool NullVideo::shutdown() { return true; }

    // Returns a nulled list.
    const VideoModeList *NullVideo::getSupportedResolutions()
    {
        VideoModeList *vidList = new VideoModeList;
        return vidList;
    }

    void NullVideo::drawAlpha(int x, int y, int w, int h, int sx, int sy, uint8_t a) const
    {
    }

    void NullVideo::drawLine(int x1, int y1, int x2, int y2, int width, Color color) {}
    void NullVideo::drawRect(int x, int y, int width, int height, Color color) {}
    void NullVideo::drawGradientRectH(int x, int y, int w, int h, Color left, Color right) {}
    void NullVideo::drawGradientRectV(int x, int y, int w, int h, Color top, Color bottom) {}
    void NullVideo::drawGradientRectD(int x, int y, int w, int h, Color top, Color middle, Color bottom) {}

    void NullVideo::drawGradientRect(int x, int y, int w, int h, GPUGradientMode m) {}
    void NullVideo::drawGradientRectHVar(int x, int y, int w, int h, Color left, Color right, int startPoint, int endPoint) {}
    void NullVideo::drawGradientRectVVar(int x, int y, int w, int h, Color top, Color bottom, int startPoint, int endPoint) {}

    void NullVideo::drawText(const char *str, int len, int x, int y, int w, int h, Color color, uint8_t mode) {}

    int NullVideo::uploadTexture(Textures::TextureObject *tObj) { return 0; }
    int NullVideo::releaseTexture(Textures::TextureObject *tObj) { return 0; }

    void NullVideo::drawSpriteObject(Sprites::SpriteObject *sObj, int x, int y, int w, int h) {}
    void NullVideo::drawTileObject(Sprites::TileObject *sObj, int x, int y, int w, int h) {}

    int NullVideo::drawTextureObject(
        Textures::TextureObject *tObj,
        int x, int y, int w, int h,
        ifloat u1, ifloat v1,
        ifloat u2, ifloat v2) { return 0; };

    int NullVideo::drawTextureObject(
        Textures::TextureObject *tObj,
        int x, int y,
        Vertex v[]) { return 0; };

}