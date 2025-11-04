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
        uint8_t x, y;
        uint8_t w, h;

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
        FONT_BOLD = (1 << 1)
    };

    class FontHeader
    {
    public:
        uint32_t magic;              // Font header magic number
        util::Hash id = 0;           // Hashed ID of font
        uint8_t version = 0;         // Font version
        uint8_t fontSize = 0;        // Pixel size of this font
        uint8_t flags = 0;           // Flags for this font, i.e. is Italic or Bold
        uint8_t spaceWidth = 0;      // Pixel width to use when entering spaces
        uint8_t tabWidth = 0;        // Pixel width to use when entering tab breaks
        int8_t lineSpacing = 0;      // Adds/removes spacing between lines if called to do so.
        int8_t baseLine = 0;         //
        int8_t kerning = 0;          // Kerning is only used for certain letters if the context calls for it.
        uint16_t numBuckets;         // Number of buckets in hashtable
        uint16_t numEntries;         // Number of glyphs in font
        uint32_t bitmapType = 0;     // Bitmap file format (if not raw bitmap)
        uint8_t bpp = 0;             // Bitdepth of the bitmap image
        uint8_t foregroundIndex = 0; // Pallete index that is used for the foreground color (if used)
        uint8_t shadowIndex = 0;     // Pallete index that is used for the shadow color (if used)
        uint8_t reserved = 0;        // Reserved
        uint32_t bitmapLength = 0;   // Length of the bitmap
        uint32_t bitmapOffset = 0;   // Relative offset of bitmap to header

        inline bool validateMagic(void) const
        {
            return (magic == GENV_BITMAP_FONT_MAGIC);
        }
    };

    class FontObject : public ObjectBase
    {
    protected:
        const FontHeader *_header;
        Textures::TextureObject *_texture = nullptr;

        const Glyph *_table = nullptr;    // Hashtable of glyph structs
        const uint8_t *_bitmap = nullptr; // Location of the bitmap entry

    public:
        FontObject(util::Hash objectID, const FontHeader *header, const Glyph *table);
        FontObject(util::Hash objectID, const FontHeader *header, const Glyph *table, Textures::TextureObject *texture);
        virtual ~FontObject();

        int print(const char *str) const;
        int printf(const char *format, ...) const;

        const Glyph get(util::UTF8CodePoint id) const;

        inline bool validateHeader() const { return _header->validateMagic(); }
        inline const FontHeader *getHeader() const { return _header; }
        virtual int uploadTexture() { return _texture->uploadTexture(); }
        Textures::TextureObject *getTexture() const { return _texture; }
    };

    class FontsetHeader
    {
    public:
        uint32_t magic = 0;         // Fontset header magic number
        util::Hash id = 0;          // Font's hashed ID
        uint8_t version = 0;        // Fontset version
        uint8_t familyLength = 0;   // Length of family name string
        uint8_t designerLength = 0; // Length of designer name string
        uint8_t reserve1 = 0;       // Reserved
        uint16_t fontCount = 0;     // Fonts in array
        uint16_t reserve2 = 0;      // Reserved
        size_t blobOffset = 0;      // Offset from first font blob

        inline bool validateMagic(void) const
        {
            return (magic == GENV_BITMAP_FONTSET_MAGIC);
        }
    };

    class FontsetObject : public ObjectBase
    {
    private:
        const FontsetHeader *_header = nullptr;
        const char *_familyName = nullptr;
        const char *_designerName = nullptr;
        FontObject **_fontList = nullptr; // Font array

    public:
        FontsetObject(const FontsetHeader &header, const char *familyName, const char *designerName, FontObject **list);
        ~FontsetObject();

        inline const char *getFamilyName() const { return _familyName; }
        inline const char *getDesignerName() const { return _designerName; }
        inline const FontsetHeader &getHeader() const { return *_header; }
        inline uint8_t size() const { return _header->fontCount; }

        inline FontObject &fontAt(uint8_t idx) const
        {
            if (idx > _header->fontCount)
                idx = 0;
            return *_fontList[idx];
        }
        FontObject &operator[](uint8_t idx) const { return this->fontAt(idx); };

        inline bool validateMagic(void) const
        {
            return _header->validateMagic();
        }

        int find(uint8_t size, uint8_t flags = FONT_NONE);
    };

    FontsetObject *loadFontsetFromFile(const char *filePath);
    FontsetObject *loadFontsetFromMem(const void *data, const size_t length);

} // namespace Fonts