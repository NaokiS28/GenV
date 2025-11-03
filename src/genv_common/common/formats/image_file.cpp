/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * soundfile.cpp - Created on 07-06-2025
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

#include <string.h>

#include "image_file.hpp"

#include "common/formats/typenames.hpp"
#include "common/objects/file.hpp"
#include "common/objects/sprite.hpp"
#include "common/objects/texture.hpp"
#include "common/services/video/color.hpp"
#include "common/util/hash.hpp"

#include "common/vendor/gifn/gifn.h"
#include "common/vendor/lodepng.h"
#include "common/vendor/vendor.h"

namespace Textures
{
    enum class ecImageFormat
    {
        IF_PNG,
        IF_BMP,
        IF_GIF,
        IF_END
    };

    struct ImageFileFormat
    {
        const char ext[8];
        ecImageFormat format;
    };

    constexpr const ImageFileFormat ifPngFile = {".png", ecImageFormat::IF_PNG};
    constexpr const ImageFileFormat ifBmpFile = {".bmp", ecImageFormat::IF_BMP};
    constexpr const ImageFileFormat ifGifFile = {".gif", ecImageFormat::IF_GIF};
    constexpr const ImageFileFormat ImageFormatList[] = {
        ifPngFile, ifBmpFile, ifGifFile};

    Textures::TextureObject *loadPNG_memory(util::Hash objectID, const uint8_t *data, size_t length)
    {
        if (data == nullptr || length == 0)
            return nullptr;

        Textures::TextureObject *tObj = Textures::createTexture(objectID);
        if (!tObj) return nullptr;

        LodePNGState state;
        LodePNGColorMode mode;
        lodepng_state_init(&state);
        lodepng_inspect(&tObj->width, &tObj->height, &state, (unsigned char *)data, length);

        mode = state.info_raw;
        if (mode.bitdepth < 16)
        {
            mode.bitdepth = 16;
        }

        unsigned int w, h;
        unsigned char **bitmap = nullptr;
        if (lodepng_decode_memory(
                bitmap, &w, &h,
                (unsigned char *)data, length,
                mode.colortype, mode.bitdepth))
        {
            delete tObj;
            lodepng_state_cleanup(&state);
            return nullptr;
        }

        if (*bitmap)
        {
            tObj->bitmap = *bitmap;
            tObj->bitmapLength = (sizeof(uint16_t) * w) * h;
            tObj->bpp = mode.bitdepth;
            tObj->width = w;
            tObj->height = h;
        }

        if (state.info_raw.colortype == LCT_PALETTE)
        {
            unsigned char *palette = new unsigned char[state.info_raw.palettesize];
            if (!palette)
            {
                delete tObj;
                lodepng_state_cleanup(&state);
                return nullptr;
            }
            memcpy(palette, state.info_raw.palette, state.info_raw.palettesize);
            // tObj->palette = palette;
            // tObj->paletteLength = state.info_raw.palettesize;
        }
        lodepng_state_cleanup(&state);
        return tObj;
    }

    Textures::TextureObject *loadGIF_memory(util::Hash objectID, const uint8_t *data, const size_t length)
    {
        if (!data || length < 13)
            return nullptr;

        Textures::TextureObject *tObj = nullptr;
        GIF_Object gif{};
        int result = gifn_load_memory(&gif, data, length);
        if (result != GIFN_OK)
            return nullptr;

        if (gif.frameCount > 1)
        {
            // TODO: Animated image support
            // Sprites::SpriteObject *tObj = Sprites::createSprite(objectID);
        }
        else
        {
            tObj = Textures::createTexture(objectID);
            if (!tObj)
            {
                gifn_cleanup(&gif);
                return nullptr;
            }

            // Allocate palette buffers
            uint32_t *palette_u32 = nullptr;
            Video::Color *vcPalette = new Video::Color[gif.header.gctSize];
            if (!vcPalette)
            {
                gifn_cleanup(&gif);
                return nullptr;
            }

            gifn_color_table_as_u32(gif.header.gct, gif.header.gctSize, &palette_u32, false);
            if (!palette_u32 || !gif.header.gctSize)
            {
                delete[] vcPalette;
                delete tObj;
                gifn_cleanup(&gif);
                return nullptr;
            }

            // Expand decoded indices to match engine’s minimum bpp (4 bits)
            // choose correct palette
            // const GIF_Color *active_ct = gif.frames[0].lct ? gif.frames[0].lct : gif.header.gct;
            int ctSize = gif.frames[0].lctSize ? gif.frames[0].lctSize : gif.header.gctSize;

            // use actual frame size, not full logical screen
            int numPix = gif.frames[0].w * gif.frames[0].h;

            int bits = (ctSize <= 2) ? 1 : (ctSize <= 4) ? 2
                                       : (ctSize <= 16)  ? 4
                                                         : 8;

            uint8_t *data = gif.frames[0].indices;
            if (bits < 4)
            {
                // Upconvert to 4bpp
                size_t nPx = (numPix / 2);
                size_t padding = (16 - ((nPx / 4) % 16));
                nPx += padding * 4;
                uint8_t *expanded = new uint8_t[nPx];
                if (!expanded)
                {
                    delete[] vcPalette;
                    delete[] palette_u32;
                    delete tObj;
                    gifn_cleanup(&gif);
                    return nullptr;
                }

                memset(expanded, 0, nPx);
                bool hp = false; // upper pixel nibble
                int b = 0;
                for (int i = 0; i < numPix; i++)
                {
                    expanded[b] |= ((gif.frames[0].indices[i] & 0xF) << (4 * hp));
                    hp = !hp;
                    if (!hp) b++;
                }
                numPix = nPx;
                data = expanded;
                bits = 4;
            }
            else
            {
                gif.frames[0].indices = nullptr;
            }

            tObj->bpp = bits;

            for (int i = 0; i < gif.header.gctSize; i++)
            {
                vcPalette[i] = {((0xFFu << 24) | palette_u32[i])};
            }

            tObj->height = gif.header.height;
            tObj->width = gif.header.width;
            tObj->loadTextureFromMem(
                data,
                numPix,
                vcPalette,
                gif.header.gctSize);

            // free temporary palette_u32 (caller owns this one)
            delete[] palette_u32;
        }

        gifn_cleanup(&gif);
        return tObj;
    }

    Textures::TextureObject *openImageFile(util::Hash objectID, const char *filePath)
    {
        if (filePath == nullptr)
            return nullptr;

        Files::FileObject *fObj = new Files::FileObject(objectID, filePath);
        if (!fObj) return nullptr;

        for (auto &af : ImageFormatList)
        {
            const char *pos = strstr(filePath, af.ext);
            if (pos != nullptr)
            {
                switch (af.format)
                {
                case ecImageFormat::IF_PNG:
                    return loadPNG_memory(
                        objectID,
                        fObj->getRawData(),
                        fObj->size());
                    break;
                case ecImageFormat::IF_BMP:
                    break;
                case ecImageFormat::IF_GIF:
                    return loadGIF_memory(
                        objectID,
                        fObj->getRawData(),
                        fObj->size());
                    break;
                default:
                    return nullptr;
                    break;
                }
            }
        }
        return nullptr;
    }

    Textures::TextureObject *openImageMemory(
        util::Hash objectID, uint32_t type, const uint8_t *data, const size_t length)
    {
        if (data == nullptr || !length)
            return nullptr;

        switch (type)
        {
        case Genv_PNG_type: return loadPNG_memory(objectID, data, length);
        case Genv_GIF_type: return loadGIF_memory(objectID, data, length);
        default: return nullptr;
        }
    }
} // namespace Textures