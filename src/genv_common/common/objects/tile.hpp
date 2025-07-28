/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * sprites.hpp - Created on 02-05-2025
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
#include <string.h>
#include "texture.hpp"
#include "common/util/ifloat.hpp"
#include "common/util/rect.h"
#include "common/util/templates.hpp"

namespace Sprites
{
    enum class Side
    {
        TopSide,
        BottomSide,
        LeftSide,
        RightSide
    };

    enum class Point
    {
        TopLeftPoint,
        TopRightPoint,
        BottomLeftPoint,
        BottomRightPoint
    };

    struct TileLUT
    {
        uint8_t r = 0;
        uint8_t c = 0;
    };

    class TileObject
    {
    protected:
        Textures::TextureObject *texture = nullptr;

        Vertex vertex[4];

        // Tile metadata for operations
        struct
        {
            bool multiTile = false;   // Set to true if tile contains variations
            bool randomTile = false;  // Set to true if tile contains variations
            bool resetOnDraw = false; // If true, resets the tile geometry once drawn
            bool cropped = false;     // If true, use texture area given in txArea to crop to
            uint16_t w = 0;           // A single tile width
            uint16_t h = 0;           // A single tile height
            uint8_t c = 0;            // Amount of tiles in texture's X direction
            uint8_t r = 0;            // Amount of tiles in texture's Y direction
            RectWH cropArea = {       // Sets the texture box to use when drawing
                0, 0, 0, 0};
            struct
            {
                const uint8_t **array = nullptr;
                uint16_t x = 0;
                uint16_t y = 0;
                uint16_t w = 0;
                uint16_t h = 0;
            } tileList;
        } meta;

        struct
        {
            uint8_t c = 0;
            uint8_t r = 0;
        } selectedTile, lastTile = {UINT8_MAX, UINT8_MAX};

        // If multi tile is enabled, atleast more than one tile must be specified
        //  and the subtile dimensions must be specified.
        inline bool useMultiTile()
        {
            return (this->meta.multiTile &&
                    (this->meta.r > 1 || this->meta.c > 1) &&
                    (this->meta.r != 0 && this->meta.c != 0) &&
                    (this->meta.w >= 1 || this->meta.h >= 1));
        }

        inline TileObject &_setTile(TileLUT lut)
        {
            meta.randomTile = false;
            return _setTile(lut.r, lut.c);
        }
        virtual TileObject &_setTile(uint8_t r, uint8_t c);
        virtual void _getNextTile();
        void _getTransformedSize(int &w, int &h);

    public:
        TileObject();
        TileObject(const char *filePath);
        virtual ~TileObject() = default;

        // Returns the internal texture pointer
        inline Textures::TextureObject *getTexture() { return texture; }

        // Sets the tile metadata. This is required for multi-tiling and random tiling
        inline virtual TileObject &setTileMeta(uint16_t w, uint16_t h, uint8_t r = 1, uint8_t c = 1)
        {
            this->meta.w = w;
            this->meta.h = h;
            this->meta.r = r; 
            this->meta.c = c;
            if (r > 1 || c > 1)
                this->meta.multiTile = true;
            resetTransform();
            return *this;
        }

        virtual int loadTextureFile(const char *filePath);
        int uploadTexture();

        // Selects a specific tile in the texture to draw. (Disables random tiling if set)
        inline TileObject &setTile(uint16_t t)
        {
            uint8_t c = (t % this->meta.c);
            uint8_t r = (t / this->meta.c);
            return this->setTile(r, c);
        }
        inline TileObject &setTile(uint8_t r, uint8_t c)
        {
            meta.randomTile = false;
            return _setTile(r, c);
        }
        inline TileObject &setTile(TileLUT lut)
        {
            meta.randomTile = false;
            return _setTile(lut.r, lut.c);
        }


        // All of the following render functions use the tile metrics for W + H and which tile to use.
        //  If this is not set, it will render the entire texture instead.

        // Draws this tile once at the specified location
        inline int draw(Coord pos) { return draw(pos.x, pos.y); }
        virtual int draw(int x, int y);

        // Repeats the tile w times in width, or h times in height, starting at x,y
        inline int repeat(RectWH rect) { return repeat(rect.x, rect.y, rect.w, rect.h); }
        virtual int repeat(int x, int y, uint16_t w, uint16_t h);

        // Fills a specified area with this tile (TODO: not clipped) (w + h in px)
        inline void fillArea(RectWH rect) { fillArea(rect.x, rect.y, rect.w, rect.h); }
        virtual void fillArea(int x, int y, int w, int h);

        // Fills the rendered screen with this tile with option offset
        virtual void fillScreen(int x = 0, int y = 0);

        inline virtual TileObject &useTileArray(const uint8_t **arr, uint16_t w, uint16_t h)
        {
            if (arr != nullptr && w > 0 && h > 0)
            {
                meta.tileList.array = arr;
                meta.tileList.w = w;
                meta.tileList.h = h;
            }
            return *this;
        }

        // Crop the texture for next draw (does not shrink draw size)
        // - If (x + w) or (y + h) would exceed tile size, the function will have no effect.
        virtual TileObject &crop(uint16_t w, uint16_t h) { return this->crop(0, 0, w, h); }
        virtual TileObject &crop(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

        // Scale the tile in total or in one axis (enlarges the draw size)
        virtual TileObject &scale(uint16_t sP, bool symetric = false) { return this->scale(sP, sP, symetric); }
        virtual TileObject &scale(uint16_t sW, uint16_t sH, bool symetric = false);

        // Skew the tile (modifies draw size)
        virtual TileObject &skew(Side side, int val);

        // Distort the tile (modifies draw size)
        virtual TileObject &distort(Point side, int x, int y);

        // Marks this transform as temporary (resets once drawn)
        inline TileObject &once()
        {
            meta.resetOnDraw = true;
            return *this;
        }

        inline Vertex *getVertex()
        {
            return vertex;
        }

        // Restores tile to original shape and size
        virtual void resetTransform();
    };
}