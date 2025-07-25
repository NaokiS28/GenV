/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * walltile.hpp - Created on 25-06-2025
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
#include "map.hpp"
#include "common/objects/tile.hpp"

// Named constants (like symbolic LUT entries)
constexpr Sprites::TileLUT TL_Corner = {0, 0};
constexpr Sprites::TileLUT TL_Left = {0, 1};
constexpr Sprites::TileLUT TL_Up = {1, 0};
constexpr Sprites::TileLUT TL_UpLeft = {1, 3};

constexpr Sprites::TileLUT TR_Corner = {0, 2};
constexpr Sprites::TileLUT TR_Up = {1, 2};
constexpr Sprites::TileLUT TR_Right = {0, 1};
constexpr Sprites::TileLUT TR_UpRight = {3, 3};

constexpr Sprites::TileLUT BL_Corner = {2, 0};
constexpr Sprites::TileLUT BL_Bottom = {1, 0};
constexpr Sprites::TileLUT BL_Left = {2, 1};
constexpr Sprites::TileLUT BL_BottomLeft = {3, 2};
constexpr Sprites::TileLUT BL_BottomShadow = {0, 3};
constexpr Sprites::TileLUT BL_LeftShadow = {3, 1};
constexpr Sprites::TileLUT BL_CornerShadow = {2, 0};

constexpr Sprites::TileLUT BR_Corner = {2, 2};
constexpr Sprites::TileLUT BR_Bottom = {1, 2};
constexpr Sprites::TileLUT BR_Right = {2, 1};
constexpr Sprites::TileLUT BR_BottomRight = {3, 0};
constexpr Sprites::TileLUT Solid = {1, 1}; // Missing art fallback

// Just easier to use LUT for the end caps (expect the bastard that is BL)
constexpr Sprites::TileLUT endCapLUT[4][5] = {
    {{0, 4}, {0, 5}, {1, 4}, {1, 5}, {0, 0}},   // Top end cap
    {{2, 4}, {2, 5}, {3, 4}, {3, 5}, {0, 0}},   // Bottom end cap
    {{2, 6}, {2, 7}, {3, 6}, {3, 7}, {3, 6}},   // Left end cap (5th tile is for shadow)
    {{0, 6}, {0, 7}, {1, 6}, {1, 7}, {0, 0}}    // Right end cap
};

class Gaunt_WallTile : private Sprites::TileObject
{
private:
     // Walls are usually made with the first 4x4 grid of tiles
    //   First 3x3 is for normal walls, to the right and underneath is corner modifiers
    //   Final 4x4 grid to the right is used for any wall that finishes a straight line.
    enum EndCap : uint8_t
    {
        Top,
        Bottom,
        Left,
        Right
    };

    constexpr Sprites::TileLUT resolveTL(uint8_t walls)
    {
        switch (walls & 0x83)
        {
        case 0x02:
        case 0x03:
            return TL_Up;
        case 0x80:
        case 0x81:
            return TL_Left;
        case 0x82:
            return TL_UpLeft;
        case 0x83:
            return Solid;
        default:
            return TL_Corner;
        }
    }

    constexpr Sprites::TileLUT resolveTR(uint8_t walls)
    {
        switch (walls & 0x0E)
        {
        case 0x02:
        case 0x06:
            return TR_Up;
        case 0x08:
        case 0x0C:
            return TR_Right;
        case 0x0A:
            return TR_UpRight;
        case 0x0E:
            return Solid;
        default:
            return TR_Corner;
        }
    }

    constexpr Sprites::TileLUT resolveBL(uint8_t walls)
    {
        switch (walls & 0xE8)
        {
        case 0x00:
        case 0x08:
            return BL_Corner;
        case 0x20:
        case 0x28:
            return BL_Bottom;
        case 0x80:
        case 0x88:
            return BL_Left;
        case 0x60:
        case 0x68:
            return BL_BottomShadow;
        case 0xC8:
            return BL_LeftShadow;
        case 0xA0:
        case 0xA8:
            return BL_BottomLeft;
        case 0x40:
        case 0xC0:
            return BL_CornerShadow;
        default:
            return Solid;
        }
    }

    constexpr Sprites::TileLUT resolveBR(uint8_t walls)
    {
        switch (walls & 0x38)
        {
        case 0x08:
        case 0x18:
            return BR_Right;
        case 0x20:
        case 0x30:
            return BR_Bottom;
        case 0x28:
            return BR_BottomRight;
        case 0x38:
            return Solid;
        default:
            return BR_Corner;
        }
    }

    // Horrendously ugly Lookup to a LUT. Walls is the result of wallCheck (starting at top left)
    //   tile is which tile from 0-3. 0 = TL, 1 = TR, 2 = BL, 3 = BR
    const Sprites::TileLUT wallConfigToLUT(const uint8_t walls, uint8_t tile)
    {
        tile %= 4;

        // Fast path for end caps
        switch (walls)
        {
        case 0x02:
        case 0x03:
        case 0x06:
        case 0x07:
            return endCapLUT[EndCap::Top][tile]; // Up
        case 0x80:
        case 0x81:
        case 0xC0:
        case 0xC1:
            return endCapLUT[EndCap::Right][tile]; // Right
        case 0x20:
        case 0x30:
        case 0x60:
        case 0x70:
            return endCapLUT[EndCap::Bottom][tile]; // Down
        case 0x0C:
        case 0x08:
        case 0x1C:
        case 0x18:
            return endCapLUT[EndCap::Left][tile]; // Left
        case 0x44:
        case 0x4C:
        case 0x48:
        case 0x58:
        case 0x5C:                                // Left shadowed
            return endCapLUT[EndCap::Left][tile == 2 ? 4 : tile];
        }

        // Select the tile-specific logic
        switch (tile)
        {
        case 0:
            return resolveTL(walls);
        case 1:
            return resolveTR(walls);
        case 2:
            return resolveBL(walls);
        case 3:
            return resolveBR(walls);
        default:
            return Solid; // Solid wall texture
        }
    }

    Gaunt_Map *map = nullptr;
    uint8_t textureVarient = 0;     // Which 
    uint8_t palleteVarient = 0;

public:
    Gaunt_WallTile();
    Gaunt_WallTile(const char *filePath);
    ~Gaunt_WallTile() = default;

    // Returns the internal texture pointer
    inline Textures::TextureObject *getTexture() { return texture; }

    // Sets the tile metadata. This is required for multi-tiling and random tiling
    inline Gaunt_WallTile &setTileMeta(uint16_t w, uint16_t h, uint8_t r = 1, uint8_t c = 1)
    {
        TileObject::setTileMeta(w, h, r, c);
        return *this;
    }

    // Sets the current map object for wall scanning
    inline Gaunt_WallTile &setMapObj(Gaunt_Map *map)
    {
        this->map = map;
        return *this;
    }

    inline int loadTextureFile(const char *filePath) { return TileObject::loadTextureFile(filePath); }
    inline int uploadTexture() { return TileObject::uploadTexture(); }

    // All of the following render functions use the tile metrics for W + H and which tile to use.
    //  If this is not set, it will render the entire texture instead.

    // Draws this tile once at the specified location
    inline int draw(Coord pos) { return draw(pos.x, pos.y); }
    int draw(int x, int y);

    inline TileObject &scale(uint16_t sP) { return TileObject::scale(sP); }
    inline TileObject &scale(uint16_t sW, uint16_t sH) { return TileObject::scale(sW, sH); }

    inline Vertex *getVertex()
    {
        return vertex;
    }
};