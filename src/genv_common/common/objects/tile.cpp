/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * sprite.cpp - Created on 09-06-2025
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

#include "tile.hpp"

#include "common/services/services.hpp"

namespace Sprites
{
    TileObject::TileObject()
    {
        resetTransform();
    }

    TileObject::TileObject(const char *filePath)
    {
        texture = Textures::createTexture(filePath);
    }

    int TileObject::draw(int x, int y)
    {
        int r = Services::getVideo()->drawTextureObject(texture, x, y, vertex);
        if (meta.resetOnDraw)
        {
            meta.resetOnDraw = false;
            _getNextTile();
            resetTransform();
        }
        lastTile = selectedTile;
        return r;
    }

    int TileObject::repeat(int x, int y, uint16_t w, uint16_t h)
    {
        int r = 0;
        int tx1 = 0, ty1 = 0, tx2 = 0, ty2 = 0, tw = 0, th = 0;
        int ih2 = 0;
        _getTransformedSize(tw, th);
        tx1 = (vertex[1].pos.x - vertex[0].pos.x);
        ty1 = (vertex[1].pos.y - vertex[0].pos.y);
        tx2 = (vertex[2].pos.x - vertex[0].pos.x);
        ty2 = (vertex[2].pos.y - vertex[0].pos.y);

        for (int ih = 0; ih < h; ih++)
        {
            for (int iw = 0; iw < w; iw++)
            {
                r = Services::getVideo()->drawTextureObject(texture, x + (tx1 * iw) + (tx2 * ih), y + (ty1 * ih2++) + (ty2 * ih), vertex);
            }
            ih2 = 0;
        }
        if (meta.resetOnDraw)
        {
            meta.resetOnDraw = false;
            resetTransform();
        }
        lastTile = selectedTile;
        return r;
    }

    TileObject &TileObject::_setTile(uint8_t r, uint8_t c)
    {
        if (useMultiTile())
        {
            // Clamp to max grid size (zero based)
            if (!(r < meta.r))
                r = (meta.r - 1);
            if (!(c < meta.c))
                c = (meta.c - 1);

            selectedTile.r = r;
            selectedTile.c = c;
        }
        else
        {
            selectedTile.r = 0;
            selectedTile.c = 0;
        }

        // Get the absolute tile coord and box
        RectWH tile = {
            meta.w * selectedTile.c,
            meta.h * selectedTile.r,
            (meta.w * selectedTile.c) + meta.w,
            (meta.w * selectedTile.r) + meta.h};

        // Crop should always be in the bounds of the current tile
        vertex[0].tex.u = tile.x + meta.cropArea.x;
        vertex[0].tex.v = tile.y + meta.cropArea.y;
        vertex[1].tex.u = (tile.w + meta.cropArea.w) - meta.cropArea.x;
        vertex[1].tex.v = tile.y + meta.cropArea.y;
        vertex[2].tex.u = tile.x + meta.cropArea.x;
        vertex[2].tex.v = (tile.h + meta.cropArea.h) - meta.cropArea.y;
        vertex[3].tex.u = (tile.w + meta.cropArea.w) - meta.cropArea.x;
        vertex[3].tex.v = (tile.h + meta.cropArea.h) - meta.cropArea.y;

        for (int i = 0; i < 4; i++)
        {
            vertex[i].tex.u = intToiFloat(vertex[i].tex.u, texture->width);
            vertex[i].tex.v = intToiFloat(vertex[i].tex.v, texture->height);
        };

        return *this;
    }

    // TODO: This just assumes you're using a 2D array.
    void TileObject::_getNextTile()
    {
        if (meta.tileList.array != nullptr && meta.tileList.w > 0 && meta.tileList.h > 0)
        {
            _setTile(meta.tileList.array[meta.tileList.y][meta.tileList.x],
                    meta.tileList.array[meta.tileList.y][meta.tileList.x]);
            if ((meta.tileList.x + 1) <= meta.tileList.w)
                meta.tileList.x++;
            else
                if ((meta.tileList.y + 1) <= meta.tileList.h)
                    meta.tileList.y++;
                else
                    meta.tileList.y = 0;
        }
    }

    int TileObject::loadTextureFile(const char *filePath)
    {
        int r = texture->loadTextureFile(filePath);
        this->meta.w = texture->width;
        this->meta.h = texture->height;
        resetTransform();
        return r;
    }

    int TileObject::uploadTexture()
    {
        return Services::getVideo()->uploadTexture(texture);
    }

    void TileObject::fillScreen(int x, int y)
    {
        fillArea(x, y, Services::getHorizontalRes(), Services::getVerticalRes());
    }

    void TileObject::fillArea(int x, int y, int w, int h)
    {
        for (int ih = 0; ih < h; ih += meta.h)
        {
            for (int iw = 0; iw < w; iw += meta.w)
            {
                draw(x + iw, y + ih);
            }
        }
    }

    // Sets the next draw operation to crop the tile. Designed to be used in a chained command
    TileObject &TileObject::crop(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
    {
        if ((x + w) > meta.w || (y + h) > meta.h || w == 0 || h == 0)
            return *this; // Invalid and would break setTile.

        // Clamping
        x %= meta.w;
        y %= meta.h;
        if (w > meta.w)
            w = meta.w;
        if (h > meta.h)
            h = meta.h;

        meta.cropArea.x = x;
        meta.cropArea.y = y;
        meta.cropArea.w = w;
        meta.cropArea.h = h;
        meta.cropped = true;

        // Get the absolute tile coord and box
        RectWH tile = {
            meta.w * selectedTile.r,
            meta.h * selectedTile.c,
            meta.w * (selectedTile.r + 1),
            meta.h * (selectedTile.c + 1)};

        // Recalculate the cropping to apply to the current tile
        vertex[0].tex.u = tile.x + meta.cropArea.x;
        vertex[0].tex.v = tile.y + meta.cropArea.y;
        vertex[1].tex.u = (tile.w + meta.cropArea.w) - meta.cropArea.x;
        vertex[1].tex.v = tile.y + meta.cropArea.y;
        vertex[2].tex.u = tile.x + meta.cropArea.x;
        vertex[2].tex.v = (tile.h + meta.cropArea.h) - meta.cropArea.y;
        vertex[3].tex.u = (tile.w + meta.cropArea.w) - meta.cropArea.x;
        vertex[3].tex.v = (tile.h + meta.cropArea.h) - meta.cropArea.y;

        for (int i = 0; i < 4; i++)
        {
            vertex[i].tex.u = intToiFloat(vertex[i].tex.u, texture->width);
            vertex[i].tex.v = intToiFloat(vertex[i].tex.v, texture->height);
        };

        return *this;
    }

    TileObject &TileObject::scale(uint16_t sW, uint16_t sH, bool symmetric)
    {
        ifloat scaleX = sW / 100.0f;
        ifloat scaleY = sH / 100.0f;

        // Calculate center of current vertices
        ifloat centerX = 0.0f;
        ifloat centerY = 0.0f;

        for (int i = 0; i < 4; ++i)
        {
            centerX += vertex[i].pos.x;
            centerY += vertex[i].pos.y;
        }

        centerX /= 4.0f;
        centerY /= 4.0f;

        for (int i = 0; i < 4; ++i)
        {
            int &x = vertex[i].pos.x;
            int &y = vertex[i].pos.y;

            if (symmetric)
            {
                // Scale around the center
                x = centerX + (x - centerX) * scaleX;
                y = centerY + (y - centerY) * scaleY;
            }
            else
            {
                // Scale relative to top-left vertex (vertex[0])
                x = vertex[0].pos.x + (x - vertex[0].pos.x) * scaleX;
                y = vertex[0].pos.y + (y - vertex[0].pos.y) * scaleY;
            }
        }

        return *this;
    }

    TileObject &TileObject::skew(Side side, int val)
    {
        switch (side)
        {
        case Side::TopSide:
            vertex[0].pos.x += val;
            vertex[1].pos.x += val;
            break;
        case Side::BottomSide:
            vertex[2].pos.x += val;
            vertex[3].pos.x += val;
            break;
        case Side::LeftSide:
            vertex[0].pos.y += val;
            vertex[2].pos.y += val;
            break;
        case Side::RightSide:
            vertex[1].pos.y += val;
            vertex[3].pos.y += val;
            break;
        default:
            break;
        }
        return *this;
    }

    TileObject &TileObject::distort(Point side, int x, int y)
    {
        switch (side)
        {
        case Point::TopLeftPoint:
            vertex[0].pos.x += x;
            vertex[0].pos.y += y;
            break;
        case Point::TopRightPoint:
            vertex[1].pos.x += x;
            vertex[1].pos.y += y;
            break;
        case Point::BottomLeftPoint:
            vertex[2].pos.x += x;
            vertex[2].pos.y += y;
            break;
        case Point::BottomRightPoint:
            vertex[3].pos.x += x;
            vertex[3].pos.y += y;
            break;
        default:
            break;
        }
        return *this;
    }

    void TileObject::resetTransform()
    {
        vertex[0] = Vertex();                                                         // Top left
        vertex[1] = Vertex(meta.w, 0, 0, 1, intToiFloat(meta.w, texture->width), 0);  // Top right
        vertex[2] = Vertex(0, meta.h, 0, 1, 0, intToiFloat(meta.h, texture->height)); // Bottom left
        vertex[3] = Vertex(meta.w, meta.h, 0, 1,
                           intToiFloat(meta.w, texture->width), intToiFloat(meta.h, texture->height)); // Bottom right
        meta.cropped = false;
        meta.cropArea = RectWH();
    }

    void TileObject::_getTransformedSize(int &w, int &h)
    {
        float minX = vertex[0].pos.x;
        float maxX = vertex[0].pos.x;
        float minY = vertex[0].pos.y;
        float maxY = vertex[0].pos.y;

        for (int i = 1; i < 4; ++i)
        {
            float x = vertex[i].pos.x;
            float y = vertex[i].pos.y;

            if (x < minX)
                minX = x;
            if (x > maxX)
                maxX = x;
            if (y < minY)
                minY = y;
            if (y > maxY)
                maxY = y;
        }

        w = maxX - minX;
        h = maxY - minY;
    }

}