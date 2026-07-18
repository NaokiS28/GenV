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
#include "common/services/video/color.hpp"
#include "common/services/video/screen.hpp"
namespace Video
{

    NullVideo::NullVideo(System::ISystem &sys) : IVideoDriver(sys)
    {
    }

    int NullVideo::init()
    {
        return GV_OK;
    }

    bool NullVideo::update()
    {
        return true;
    }

    bool NullVideo::reset()
    {
        return true;
    }

    bool NullVideo::beginRender(Video::Screen &screen)
    {
        return true;
    }

    bool NullVideo::endRender(Video::Screen &screen)
    {
        return true;
    }

    void NullVideo::shutdown()
    {
    }

    // Returns a nulled list.
    const VESA::VideoModeList *NullVideo::getSupportedResolutions(Video::Screen &screen)
    {
        return &VESA::BlankVideoList;
    }

    void NullVideo::drawAlpha(Video::Screen &screen, int x, int y, int w, int h, int sx, int sy, uint8_t a) const
    {
    }

    void NullVideo::drawLine(Video::Screen &screen, int x1, int y1, int x2, int y2, int width, Color color) {}
    void NullVideo::drawGradientLine(Video::Screen &screen, int x1, int y1, int x2, int y2, int width, Color c1, Color c2) {}
    void NullVideo::drawRect(Video::Screen &screen, int x, int y, int width, int height, Color color) {}
    void NullVideo::drawGradientRectH(Video::Screen &screen, int x, int y, int w, int h, Color left, Color right) {}
    void NullVideo::drawGradientRectV(Video::Screen &screen, int x, int y, int w, int h, Color top, Color bottom) {}
    void NullVideo::drawGradientRectD(Video::Screen &screen, int x, int y, int w, int h, Color top, Color middle, Color bottom) {}

    void NullVideo::drawGradientRect(Video::Screen &screen, int x, int y, int w, int h, GPUGradientMode m) {}
    void NullVideo::drawGradientRectHVar(Video::Screen &screen, int x, int y, int w, int h, Color left, Color right, int startPoint, int endPoint) {}
    void NullVideo::drawGradientRectVVar(Video::Screen &screen, int x, int y, int w, int h, Color top, Color bottom, int startPoint, int endPoint) {}

    int NullVideo::drawText(Video::Screen &screen, const char *str, int x, int y, int w, int h, Color color, uint8_t mode)
    {
        return 0;
    }

    int NullVideo::uploadTexture(Textures::TextureObject *tObj)
    {
        return 0;
    }
    int NullVideo::releaseTexture(Textures::TextureObject *tObj)
    {
        return 0;
    }

    void NullVideo::drawSpriteObject(Video::Screen &screen, Sprites::SpriteObject *sObj, int x, int y, int w, int h) {}
    void NullVideo::drawTileObject(Video::Screen &screen, Sprites::TileObject *sObj, int x, int y, int w, int h) {}

    int NullVideo::drawTextureObject(
        Video::Screen &screen,
        const Textures::TextureObject *tObj,
        int x, int y, int w, int h,
        ifloat u1, ifloat v1,
        ifloat u2, ifloat v2)
    {
        return 0;
    };

    int NullVideo::drawTextureObject(
        Video::Screen &screen,
        const Textures::TextureObject *tObj,
        int x, int y,
        Vertex v[])
    {
        return 0;
    };

} // namespace Video
