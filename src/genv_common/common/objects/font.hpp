/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * fontobj.hpp - Created on 11-05-2025
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

/*
    General manager for Bitmap fonts. Does not support vector fonts.
*/

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "common/objects/object.hpp"
#include "texture.hpp"
#include "common/util/hash.hpp"
#include "common/util/templates.hpp"
#include "common/util/string.hpp"
#include "common/services/video/color.hpp"
#include "common/logger/log.hpp"
#include "common/return_codes.hpp"

#define LOG_FOBJ(fmt, ...) LOG("fontobj", fmt __VA_OPT__(, ) __VA_ARGS__)
#define FO_ERROR(code) GV_ERROR(GV_SERVICE_VIDEO, GV_CATEGORY_GENERIC, code)

namespace Fonts
{
    static constexpr const Video::Color font_shadow(0, 0, 0, 128);
    static constexpr const util::Hash GENV_FONT_OBJ_TYPENAME = "GenVFontObject"_h;
    static constexpr const uint32_t GENV_BITMAP_FONTSET_MAGIC = "GVBF"_c;
    static constexpr const uint32_t GENV_BITMAP_FONT_MAGIC = "GVFO"_c;

    static constexpr size_t METRICS_CODE_POINT_BITS = 21;
    static constexpr util::UTF8CodePoint FONT_INVALID_CHAR = 0xfffd;
    using CharacterSize = uint32_t;

    class Glyph
    {
    public:
        uint32_t c; // Unicode codepoint
        int x, y;
        int w, h;

        inline util::Hash getHash(void) const
        {
            return c & ((1 << METRICS_CODE_POINT_BITS) - 1);
        }
        inline uint32_t getChained(void) const
        {
            return c >> METRICS_CODE_POINT_BITS;
        }
    };

    enum FontFlags : uint8_t
    {
        FONT_NONE,
        FONT_ITALIC = (1 << 0),
        FONT_BOLD = (1 << 1),
        FONT_INDEXED = (1 << 2), // Uses indexed colours which can support changing the font color
    };

    class FontHeader
    {
    public:
        uint32_t magic;
        const char *name = nullptr;
        const char *designer = nullptr;
        util::Hash id = 0;
        uint8_t flags = 0;

        int fontSize = 0;
        int lineSpacing = 0;    // Adds/removes spacing between lines if called to do so.
        int kerning = 0;        // Kerning is only used for certain letters if the context calls for it.
        uint8_t spaceWidth = 0; // Pixel width to use when entering spaces
        uint8_t tabWidth = 0;   // Pixel width to use when entering tab breaks

        uint32_t bitmapType;     // Bitmap file format (if not raw bitmap)
        uint8_t bpp;             // Bitdepth of the bitmap image
        uint8_t foregroundIndex; // Pallete index that is used for the foreground color (if used)
        uint8_t shadowIndex;     // Pallete index that is used for the shadow color (if used)

        size_t bitmapLength;
        uint8_t *bitmap;

        size_t tableLength;
        Glyph *table = nullptr;

        inline bool validateMagic(void) const
        {
            return (magic == GENV_BITMAP_FONT_MAGIC);
        }
    };

    class FontObject : public ObjectBase
    {
    protected:
        FontHeader header;
        Textures::TextureObject *texture = nullptr;

    public:
        FontObject(util::Hash objectID);
        FontObject(util::Hash objectID, FontHeader _header);
        FontObject(util::Hash objectID, FontHeader _header, Textures::TextureObject *_texture);
        virtual ~FontObject();

        int print(const char *str) const;
        int printf(const char *format, ...) const;

        const Glyph get(util::UTF8CodePoint id) const;

        inline const FontHeader &getHeader() const { return header; }
        virtual int uploadTexture() const { return texture->uploadTexture(); }
        const Textures::TextureObject *getTexture() const { return texture; }
    };

    class GenvFontset
    {
    public:
        const uint32_t magic;
        const char *familyName;
        const uint8_t fontCount;
        const uint8_t *fontSizes;

        FontObject *fontList;

        inline bool validateMagic(void) const
        {
            return (magic == GENV_BITMAP_FONTSET_MAGIC);
        }
    };

    int loadFontFromFile(FontObject **fObj, const char *filePath);
    int loadFontFromMem(FontObject **fObj, const uint8_t *data, const size_t length);
    int loadFontFromMem(FontObject **fObj, Textures::TextureObject *tObj, FontHeader _header);

} // namespace Fonts