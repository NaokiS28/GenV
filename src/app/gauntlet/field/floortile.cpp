/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * floortile.cpp - Created on 25-06-2025
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

#include "floortile.hpp"
#include "common/services/services.hpp"

void Gaunt_FloorTile::_getNextTile()
{
}

Gaunt_FloorTile::Gaunt_FloorTile() : TileObject()
{
}
Gaunt_FloorTile::Gaunt_FloorTile(const char *filePath) : TileObject(filePath)
{
}

int Gaunt_FloorTile::draw(int x, int y, uint16_t tx, uint16_t ty)
{
    if (meta.tileList.array != nullptr && meta.tileList.w > 0 && meta.tileList.h > 0)
    {
        uint16_t s = 0, c = 0, tw = 0, th = 0;
        c = (2 * ((meta.tileList.array[ty][tx] & 0xF0) >> 4)); // Random column
        s = (meta.tileList.array[ty][tx] & 0x0F);              // Shadow style
        tw = vertex[1].pos.x;                                  // Tile size (post transform)
        th = vertex[2].pos.y;                                  // Tile size (post transform)

        // Draw top right corner of this tile. Is never shaded.
        // Top left
        _setTile(floorTileLUT[s][0].r, c + floorTileLUT[s][0].c);
        Services::getVideo()->drawTextureObject(texture, x, y, vertex);
        // Top Right (Always unshadowed)
        _setTile(floorTileLUT[s][1].r, c + floorTileLUT[s][1].c);
        Services::getVideo()->drawTextureObject(texture, x + tw, y, vertex);
        // Bottom left
        _setTile(floorTileLUT[s][2].r, c + floorTileLUT[s][2].c);
        Services::getVideo()->drawTextureObject(texture, x, y + th, vertex);
        // Bottom Right
        _setTile(floorTileLUT[s][3].r, c + floorTileLUT[s][3].c);
        Services::getVideo()->drawTextureObject(texture, x + tw, y + th, vertex);
    }

    if (meta.resetOnDraw)
    {
        meta.resetOnDraw = false;
        _getNextTile();
        resetTransform();
    }
    lastTile = selectedTile;
    return 0;
}

int Gaunt_FloorTile::repeat(int x, int y, uint16_t w, uint16_t h)
{
    if (meta.tileList.array != nullptr && meta.tileList.w > 0 && meta.tileList.h > 0)
    {
        for (uint16_t ih = 0; ih < h; ih++)
        {
            for (uint16_t iw = 0; iw < w; iw++)
            {
                uint16_t s = 0, c = 0, tw = 0, th = 0;
                c = (2 * ((meta.tileList.array[ih][iw] & 0xF0) >> 4)); // Random column
                s = (meta.tileList.array[ih][iw] & 0x0F);              // Shadow style
                tw = vertex[1].pos.x;                                  // Tile size (post transform)
                th = vertex[2].pos.y;                                  // Tile size (post transform)

                if (s == Gaunt_FloorTile::Shadow::Hidden_Floor)
                {
                    // Under wall, don't draw this tile.
                    continue;
                }

                int tx = (x + ((tw * 2) * iw));
                int ty = (y + ((th * 2) * ih));

                // Draw top right corner of this tile. Is never shaded.
                // Top left
                _setTile(floorTileLUT[s][0].r, c + floorTileLUT[s][0].c);
                Services::getVideo()->drawTextureObject(texture, tx, ty, vertex);
                // Top Right (Always unshadowed)
                _setTile(floorTileLUT[s][1].r, c + floorTileLUT[s][1].c);
                Services::getVideo()->drawTextureObject(texture, tx + tw, ty, vertex);
                // Bottom left
                _setTile(floorTileLUT[s][2].r, c + floorTileLUT[s][2].c);
                Services::getVideo()->drawTextureObject(texture, tx, ty + th, vertex);
                // Bottom Right
                _setTile(floorTileLUT[s][3].r, c + floorTileLUT[s][3].c);
                Services::getVideo()->drawTextureObject(texture, tx + tw, ty + th, vertex);
            }
        }
    }
    if (meta.resetOnDraw)
    {
        meta.resetOnDraw = false;
        _getNextTile();
        resetTransform();
    }
    lastTile = selectedTile;
    return 0;
}