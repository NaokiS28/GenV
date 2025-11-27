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

#include "sprite.hpp"

#include "common/objects/texture.hpp"
#include "common/services/services.hpp"
#include "common/util/hash.hpp"

namespace Sprites
{
    SpriteObject::SpriteObject(util::Hash objectID)
    {
        texture = getServiceManager()->getVideo()->createTexture(objectID);
    }

    SpriteObject::SpriteObject(util::Hash objectID, Textures::TextureObject *tObj)
    {
        texture = tObj;
    }

    SpriteObject::SpriteObject(util::Hash objectID, const char *filePath)
    {
        texture = getServiceManager()->getVideo()->createTexture(objectID, filePath);
    }

    void SpriteObject::draw(int x, int y, int w, int h)
    {
        return getServiceManager()->getVideo()->drawSpriteObject(this, x, y, w, h);
    }

    int SpriteObject::loadTextureFromFile(const char *filePath)
    {
        return texture->loadTextureFromFile(filePath);
    }

    int SpriteObject::uploadTexture()
    {
        return getServiceManager()->getVideo()->uploadTexture(texture);
    }

    SpriteObject *createSprite(util::Hash objectID, Textures::TextureObject *tObj)
    {
        if (!tObj) return nullptr;
        SpriteObject *sObj = new SpriteObject(objectID, tObj);
        return sObj;
    }

    SpriteObject *createSprite(util::Hash objectID, const char *filePath)
    {
        return nullptr;
    }
} // namespace Sprites