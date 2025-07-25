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
#include "texture.hpp"
#include "common/util/ifloat.hpp"

class SpriteObject
{
private:
    TextureObject *tex;

    struct SpritePosition
    {
        ifloat x, y, z, rhw; // screen-space position, z=0.0f, rhw=1.0f
        ifloat u, v;         // texture coordinates
    } pos;
    
    uint32_t color; // diffuse color (ARGB)

public:
};