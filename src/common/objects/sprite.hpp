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
#include "common/util/rect.h"
#include "common/util/templates.hpp"

namespace Sprites
{
    struct SpritePosition
    {
        ifloat x, y, z, rhw; // screen-space position, z=0.0f, rhw=1.0f
        ifloat u, v;         // texture coordinates
    };

    class SpriteObject
    {
    private:
        Textures::TextureObject *texture = nullptr;

        SpritePosition pos = {0, 0, 0, 0, 0, 0};

        uint32_t color = 0; // diffuse color (ARGB)

    public:
        SpriteObject();
        SpriteObject(const char *filePath);
        virtual ~SpriteObject() = default;

        inline Textures::TextureObject *getTexture()
        {
            return texture;
        }

        inline SpritePosition getPosition()
        {
            return pos;
        }

        inline void draw()
        {
            draw(pos.x, pos.y, texture->width, texture->height);
        }
        inline void draw(int x, int y)
        {
            draw(x, y, texture->width, texture->height);
        }
        virtual void draw(int x, int y, int w, int h);

        inline void drawScaled(uint8_t sW, uint8_t sH)
        {
            if(sW > 100) sW = 100;
            if(sH > 100) sH = 100;
            draw(
                pos.x,
                pos.y,
                util::percentOf(sW, texture->width),
                util::percentOf(sH, texture->height));
        }

        inline void drawScaled(uint8_t sP){
            drawScaled(sP, sP);
        }

        inline void setPosition(Coord position)
        {
            pos.x = position.x;
            pos.y = position.y;
            pos.z = position.z;
        }

        inline void setPosition(unsigned int x, unsigned int y, unsigned int z = 0)
        {
            setPosition(Coord(x, y, z));
        }

        inline void transformX(int x) { pos.x += x; }
        inline void transformY(int y) { pos.y += y; }
        inline void transformZ(int z) { pos.z += z; }

        inline void setX(int x) { pos.x = x; }
        inline void setY(int y) { pos.y = y; }
        inline void setZ(int z) { pos.z = z; }

        virtual int loadTextureFile(const char *filePath);
        int uploadTexture();
    };
}