/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * floortile.hpp - Created on 25-06-2025
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

#include "common/objects/tile.hpp"

class Gaunt_FloorTile : private Sprites::TileObject
{
public:
    enum Shadow : uint8_t
    {
        None,
        Left_Diag,
        Left_Solid,
        Bottom_Diag,
        Bottom_Solid,
        Left_Bottom_Solid,
        Lower_Left_Corner,
        Hidden_Floor = 0x0F
    };

private:
    // Gauntlet specific. Defines what row/cols to use when drawing random tiles/shadows
    Sprites::TileLUT floorTileLUT[7][4] = {
        // TL, TR, BL, BR
        {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
        {{4, 0}, {0, 1}, {5, 0}, {1, 1}},
        {{4, 1}, {0, 1}, {5, 1}, {0, 1}},
        {{0, 0}, {0, 1}, {2, 0}, {2, 1}},
        {{0, 0}, {0, 1}, {3, 0}, {3, 1}},
        {{4, 1}, {0, 1}, {5, 1}, {3, 1}},
        {{0, 0}, {0, 1}, {3, 0}, {1, 1}}};

    void _getNextTile() override;

public:
    Gaunt_FloorTile();
    Gaunt_FloorTile(const char *filePath);
    ~Gaunt_FloorTile() = default;

    // Returns the internal texture pointer
    inline Textures::TextureObject *getTexture() { return texture; }

    // Sets the tile metadata. This is required for multi-tiling and random tiling
    inline TileObject &setTileMeta(uint16_t w, uint16_t h, uint8_t r = 1, uint8_t c = 1)
    {
        TileObject::setTileMeta(w, h, r, c);
        return *this;
    }

    inline int loadTextureFile(const char *filePath) { return TileObject::loadTextureFile(filePath); }
    inline int uploadTexture() { return TileObject::uploadTexture(); }

    // All of the following render functions use the tile metrics for W + H and which tile to use.
    //  If this is not set, it will render the entire texture instead.

    // Draws this tile once at the specified location
    inline int draw(Coord pos, uint16_t tx, uint16_t ty) { return draw(pos.x, pos.y, tx, ty); }
    int draw(int x, int y, uint16_t tx, uint16_t ty);

    // Repeats the tile w times in width, or h times in height, starting at x,y
    inline int repeat(RectWH rect) { return repeat(rect.x, rect.y, rect.w, rect.h); }
    int repeat(int x, int y, uint16_t w, uint16_t h) override;

    // Fills a specified area with this tile (TODO: not clipped) (w + h in px)
    // inline void fillArea(RectWH rect) { fillArea(rect.x, rect.y, rect.w, rect.h); }
    // void fillArea(int x, int y, int w, int h) override;
    inline TileObject &scale(uint16_t sP, bool symetric = false) { return TileObject::scale(sP, symetric); }
    inline TileObject &scale(uint16_t sW, uint16_t sH, bool symetric = false) { return TileObject::scale(sW, sH, symetric); }

    // Provide the tile object with an array to use for using as a LUT
    inline TileObject &useTileArray(const uint8_t **arr, uint16_t w, uint16_t h)
    {
        return TileObject::useTileArray(arr, w, h);
    }

    inline Vertex *getVertex()
    {
        return vertex;
    }
};