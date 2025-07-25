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

#include "common/vendor/vendor.h"
#include "common/services/services.hpp"
#include "texture/missingtex.h"
#include "common/util/hash.hpp"

namespace Textures
{
    DefaultTexture::DefaultTexture()
    {
        TextureObject::bitmap = stbi_load_from_memory(
            missingTextureImg,
            missingTextureSize,
            &width,
            &height,
            &bpp,
            0);
        TextureObject::textureID = "DefaultTexture"_h;
    }

    TextureObject::TextureObject() {}

    TextureObject::~TextureObject()
    {
        Services::getVideo()->releaseTexture(this);
    }

    TextureObject::TextureObject(const char *filePath)
    {
        loadTextureFile(filePath);
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
                    &n,
                    4);

                bitmapLength = ((sizeof(uint32_t) * width) * height);
                textureID = Files::getFileNameHash(this->file);

                if (bitmap != nullptr)
                    return Files::FO_OKAY;
            }
        }

        return Files::FO_ERROR_BADOBJECT;
    }

    int TextureObject::uploadTexture()
    {
        if (!textureID)
            Files::getFileNameHash(this->file);
        return Services::getVideo()->uploadTexture(this);
    }

    TextureObject *createTexture(const char *filePath)
    {
        TextureObject *tObj = new TextureObject;
        if (tObj != nullptr)
        {
            if (tObj->loadTextureFile(filePath) == Files::FO_OKAY)
                return tObj;
            else
                delete tObj;
        }

        static DefaultTexture txDefault;
        return &txDefault;
    }
}