/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * map.cpp - Created on 25-06-2025
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
#include "map.hpp"

void Gaunt_Map::generateFloor()
{
    floorMap = new uint8_t *[Gaunt_MapSizeH];
    for (unsigned int h = 0; h < Gaunt_MapSizeH; h++)
    {
        floorMap[h] = new uint8_t[Gaunt_MapSizeW];
        if (floorMap[h] != nullptr)
        {
            for (unsigned int w = 0; w < Gaunt_MapSizeW; w++)
            {
                uint8_t c = Services::random(0, 4);
                c ^= (uint8_t)Services::random(0, 4);
                uint8_t walls = shadowCheck(w, h);
                this->floorMap[h][w] = (0 | ((c & 0x0F) << 4) | (walls & 0x0F));
            }
        }
    }
}