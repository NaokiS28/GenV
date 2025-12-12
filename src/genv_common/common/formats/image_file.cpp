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
#include "common/objects/texture.hpp"
#include "common/services/services.hpp"
#include "common/services/video/color.hpp"
#include "common/util/hash.hpp"

#include "common/vendor/gifn/gifn.h"
#include "common/vendor/lodepng.h"
#include "common/logger/log.hpp"

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
        ServiceManager &services = *getServiceManager();

        if (data == nullptr || length == 0)
            return nullptr;

        Textures::TextureObject *tObj = Textures::createTexture(objectID);
        if (!tObj) return nullptr;

        LodePNGState state;
        unsigned int w = 0, h = 0;
        uint8_t *bitmap = nullptr;

        auto fail = [&tObj, &state, &bitmap](int err)
        {
            LOG("pngDecode", "Failed to open PNG file, error:", err);
            delete tObj;
            if (bitmap) delete[] bitmap;
            lodepng_state_cleanup(&state);
            return (Textures::TextureObject *)nullptr;
        };

        lodepng_state_init(&state);
        if (lodepng_inspect(&w, &h, &state, data, length))
            return fail(1);

        if (lodepng_decode(&bitmap, &w, &h, &state, data, length))
            return fail(2);

        // TODO: Pallete loading might be borked.
        if (state.info_png.color.colortype == LCT_PALETTE)
        {
            const LodePNGColorMode *pal = &state.info_png.color;
            if (pal->palettesize == 0)
                return fail(3);

            auto *paletteBuffer = new Video::Color[pal->palettesize];
            if (!paletteBuffer)
                return fail(4);

            for (unsigned i = 0; i < pal->palettesize; i++)
            {
                uint8_t r = pal->palette[i * 4 + 0];
                uint8_t g = pal->palette[i * 4 + 1];
                uint8_t b = pal->palette[i * 4 + 2];
                uint8_t a = pal->palette[i * 4 + 3];
                paletteBuffer[i] = {
                    a, r, g, b};
            }

            tObj->bpp = state.info_raw.bitdepth;
            tObj->palette = paletteBuffer;
            tObj->paletteLength = pal->palettesize;

            // TODO: Dont do this
            tObj->bitmapLength = services.gfx_size(w * h);
            uint8_t *dst = (uint8_t *)services.gfx_alloc(w * h);
            memcpy(dst, bitmap, tObj->bitmapLength);
            delete[] bitmap;
            // Seriously

            tObj->bitmap = dst;
        }
        else
        {
            // TODO: Dont do this
            int bitmapLen = services.gfx_size(w * h * 2);
            uint8_t *dst = (uint8_t *)services.gfx_alloc(w * h * 2);
            if (!dst)
                return fail(4);

            memset(dst, 0, bitmapLen);
            unsigned numPixels = w * h;
            for (unsigned i = 0; i < numPixels; i++)
            {
                uint8_t r = bitmap[i * 4 + 0];
                uint8_t g = bitmap[i * 4 + 1];
                uint8_t b = bitmap[i * 4 + 2];
                uint8_t a = bitmap[i * 4 + 3];

                // Alpha bit only for semi-transparent
                uint16_t px = ((a < 127 ? 0x8000 : 0) |
                               ((r & 0xF8) << 7) |
                               ((g & 0xF8) << 2) |
                               ((b & 0xF8) >> 3));
                dst[i * 2 + 0] = (px & 0xFF);
                dst[i * 2 + 1] = ((px >> 8) & 0xFF);
            }
            delete[] bitmap;
            bitmap = dst;

            tObj->bitmap = bitmap;
            tObj->bitmapLength = bitmapLen;
            tObj->bpp = 16;
        }

        tObj->width = w;
        tObj->height = h;

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

        // if (gif.frameCount > 1)
        //{
        // TODO: Animated image support
        // Sprites::SpriteObject *tObj = Sprites::createSprite(objectID);
        //}
        // else
        //{
        tObj = Textures::createTexture(objectID);
        if (!tObj)
        {
            gifn_cleanup(&gif);
            return nullptr;
        }

        // Allocate palette buffers
        uint32_t *palette_u32 = gifn_color_table_as_u32(gif.header.gct, gif.header.gctSize, false);
        Video::Color *vcPalette = new Video::Color[gif.header.gctSize];
        if (!vcPalette || !palette_u32 || !gif.header.gctSize)
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

        uint8_t *workBuffer = gif.frames[0].indices;
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
            workBuffer = expanded;
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
            workBuffer,
            numPix,
            vcPalette,
            gif.header.gctSize);

        // free temporary palette_u32 (caller owns this one)
        delete[] palette_u32;
        //}

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