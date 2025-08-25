/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * texture.cpp - Created on 09-06-2025
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

#include "texture.hpp"

#include "texture/missingtex.h"

#include "common/vendor/vendor.h"
#include "common/services/services.hpp"

namespace Textures
{
    TextureObject::~TextureObject()
    {
        Services::getVideo()->releaseTexture(this);
        delete[] clut;
        delete[] bitmap;
    }

    TextureObject::TextureObject(util::Hash objectID) : ObjectBase()
    {
        setObjectID(objectID);
        setObjectType(GENV_TEXTURE_OBJ_TYPENAME);
    }

    TextureObject::TextureObject(util::Hash objectID, const char *filePath) : ObjectBase()
    {
        setObjectID(objectID);
        loadTextureFile(filePath);
        setObjectType(GENV_TEXTURE_OBJ_TYPENAME);
    }

    int TextureObject::loadTextureFile(const char *filePath)
    {
        if (file == nullptr)
            file = new Files::FileObject();

        if (file != nullptr)
        {
            int result = file->openFile(filePath, false);
            if (result == Files::FO_OKAY)
            {
                bitmap = stbi_load_from_memory(
                    file->getRawDataObj()->getRawData(),
                    file->getRawDataObj()->getDataLen(),
                    &width,
                    &height,
                    &bpp,
                    4);

                bitmapLength = ((sizeof(uint32_t) * width) * height);
                setObjectID(Files::getFileNameHash(this->file));

                if (bitmap != nullptr)
                    return Files::FO_OKAY;
            }
        }
        return Files::FO_ERROR_BADOBJECT;
    }

    int TextureObject::uploadTexture()
    {
        if (!getObjectID())
            setObjectID(Files::getFileNameHash(this->file));
        return Services::getVideo()->uploadTexture(this);
    }

    TextureObject *createDefaultTexture()
    {
        TextureObject *tObj = createTexture("DefaultTexture"_h);
        if (!tObj)
            return nullptr;

        tObj->bitmap = stbi_load_from_memory(
            missingTextureImg,
            missingTextureSize,
            &tObj->width,
            &tObj->height,
            &tObj->bpp,
            0);
        return tObj;
    }

    TextureObject *createTexture(util::Hash objectID)
    {
        TextureObject *tObj = Services::getVideo()->createTexture(objectID);
        return tObj;
    }

    TextureObject *createTexture(util::Hash objectID, const char *filePath)
    {
        TextureObject *tObj = Services::getVideo()->createTexture(objectID);
        if (tObj != nullptr)
        {
            if (tObj->loadTextureFile(filePath) == Files::FO_OKAY)
                return tObj;
            else
            {
                delete tObj;
                tObj = nullptr;
            }
        }
        return tObj;
    }
} // namespace Textures