/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * walltile.cpp - Created on 25-06-2025
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

#include <stdint.h>
#include "common/services/services.hpp"
#include "walltile.hpp"

Gaunt_WallTile::Gaunt_WallTile() : TileObject()
{
}

Gaunt_WallTile::Gaunt_WallTile(const char *filePath) : TileObject(filePath)
{
}

int Gaunt_WallTile::draw(int x, int y)
{
    uint8_t s = this->map->wallCheck(x, y); // Walls must check if they are connected to another wall, inc. destroyable walls.
    uint16_t w = vertex[1].pos.x;           // Tile size (post transform)
    uint16_t h = vertex[2].pos.y;           // Tile size (post transform)

    int tx = ((w * 2) * x);
    int ty = ((h * 2) * y);

    for (int d = 0; d < 4; d++)
    {
        _setTile(wallConfigToLUT(s, d));
        Services::getVideo()->drawTextureObject(texture, tx + (w * (d & 0x01)), ty + (h * ((d & 0x02) >> 1)), vertex);
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