/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * textures.hpp - Created on 02-05-2025
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

#include "object.hpp"
#include "file.hpp"

namespace Textures
{
    static constexpr const char *GENV_TEXTURE_OBJ_TYPENAME = "GenVTextureObject";

    enum BitsPerPixel : uint8_t
    {
        INVALID,
        BPP_1BIT = 1,
        BPP_2BIT = 2,
        BPP_4BIT = 4,
        BPP_8BIT = 8,
        BPP_16BIT = 16,
        BPP_24BIT = 24
    };

    class TextureObject : public ObjectBase
    {
    private:
        Files::FileObject *file = nullptr;

    public:
        int width = 0;
        int height = 0;
        int bpp = 0;

        size_t bitmapLength = 0;
        const uint8_t *bitmap = nullptr;
        const uint8_t *clut = nullptr;

        TextureObject() : ObjectBase() {}
        TextureObject(const char *filePath);
        virtual ~TextureObject();

        virtual int loadTextureFile(const char *filePath);
        virtual int uploadTexture();
        virtual size_t getTextureSize()
        {
            return bitmapLength;
        }
    };

    // Failsafe texture if a texture object fails to load or otherwise cannot be used
    class DefaultTexture : public TextureObject
    {
    public:
        DefaultTexture();
        ~DefaultTexture() override {} // If this isnt overiding the default texture destructor, program will seg fault when closing.
    };

    TextureObject *createTexture();
    TextureObject *createTexture(const char *filePath);
} // namespace Textures

// Is pow(bpp,2), really.
static constexpr const size_t genv_bpp(uint8_t bpp)
{
    size_t r = 2;
    if (bpp < 2)
        return bpp;
    for (int p = 1; p < bpp; p++)
    {
        r <<= 1;
    }
    return r;
}