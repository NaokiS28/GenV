/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * font.cpp - Created on 22-08-2025
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

#include "font.hpp"

#include "common/formats/image_file.hpp"
#include "common/logger/error_strings.hpp"
#include "common/objects/object.hpp"
#include "common/objects/texture.hpp"
#include "common/return_codes.hpp"
#include "common/util/string.hpp"
#include "common/util/templates.hpp"

namespace Fonts
{
    FontObject::FontObject(util::Hash objectID, const FontHeader *header, const Glyph *table)
        : ObjectBase(objectID)
    {
        setObjectType(GENV_FONT_OBJ_TYPENAME);
        _header = header;
        _table  = table;
    }

    FontObject::FontObject(util::Hash objectID, const FontHeader *header, const Glyph *table, Textures::TextureObject *texture)
        : ObjectBase(objectID)
    {
        setObjectType(GENV_FONT_OBJ_TYPENAME);
        _texture = texture;
        _header  = header;
        _table   = table;
    }

    FontObject::~FontObject()
    {
        if (_header)
            delete _header;
        if (_texture)
            delete _texture;
    }

    FontsetObject::FontsetObject(
        const FontsetHeader &header,
        const char *familyName,
        const char *designerName,
        FontObject **list)
        : ObjectBase(header.id),
          _header(&header),
          _familyName(familyName),
          _designerName(designerName),
          _fontList(list)
    {
    }

    FontsetObject::~FontsetObject()
    {
    }

    int FontsetObject::find(uint8_t size, uint8_t flags)
    {
        int bestIndex      = FO_ERROR(GV_ERR_INCOMPATIBLE_PARAMS);
        uint32_t bestScore = 0xFFFFFFFF; // lower score = better fit

        for (uint16_t i = 0; i < _header->fontCount; i++)
        {
            const FontHeader &f_header = *_fontList[i]->getHeader();
            // Rule 1: Skip if the fontsize is larger than requested
            if (size < f_header.fontSize)
                continue;

            // Compute how well it fits: prioritize matching exactly, then closeness
            uint16_t ds = size - f_header.fontSize;
            // TODO: Change this so it prefers non-bold/italic fonts when requested
            uint16_t df    = flags - f_header.flags;
            uint32_t score = (uint32_t)ds * df + (uint32_t)ds * df;

            // Prefer exact match if found - TODO: Entirely broken because it was designed for video modes, not fonts
            if (ds == 0 && df == 0)
            {
                bestIndex = i;
                bestScore = 0;
                break;
            }

            if (score < bestScore)
            {
                bestScore = score;
                bestIndex = i;
            }
        }

        // If no suitable mode found under or equal to requested, pick the closest larger font
        if (bestIndex == -1)
        {
            uint32_t closestSize = 0;
            for (uint16_t i = 0; i < _header->fontCount; i++)
            {
                const FontHeader &f_header = *_fontList[i]->getHeader();
                if (f_header.fontSize < closestSize)
                {
                    bestIndex   = i;
                    closestSize = f_header.fontSize;
                }
            }
        }

        uint32_t result = 0;
        if (bestIndex >= 0)
            result = bestIndex;
        return result;
    }

    FontsetObject *loadFontsetV1(const FontsetHeader *fsHeader, const uint8_t *base, size_t length)
    {
        int error  = 0;
        size_t pos = sizeof(FontsetHeader);

        const char *familyName   = reinterpret_cast<const char *>(base + pos);
        const char *designerName = reinterpret_cast<const char *>(base + pos + fsHeader->familyLength);
        pos += fsHeader->familyLength + fsHeader->designerLength;

        const size_t offsetsSize    = static_cast<size_t>(fsHeader->fontCount) * sizeof(uint32_t);
        const uint8_t *offsetsBytes = base + pos;
        pos += offsetsSize;

        FontObject **fontBlobs = new FontObject *[fsHeader->fontCount] {};
        if (!fontBlobs) return nullptr;

        // Clear the allocated objects if any failures occur
        auto deleteAndFail = [](FontObject **table, const FontsetHeader *fsHeader) -> FontsetObject *
        {
            for (int i = 0; i < fsHeader->fontCount; i++)
            {
                if (table[i] != nullptr)
                    delete table[i];
            }
            delete[] table;
            return nullptr;
        };

        for (uint16_t i = 0; i < fsHeader->fontCount; i++)
        {
            // Read a uint32_t offset safely (alignment-agnostic)
            uint32_t fontBaseOff = 0;
            memcpy(&fontBaseOff, offsetsBytes + i * sizeof(uint32_t), sizeof(uint32_t));

            const uint8_t *fontBase    = (base + fontBaseOff);
            const FontHeader *ftHeader = reinterpret_cast<const FontHeader *>(fontBase);
            util::assertAligned<uint32_t>(ftHeader);
            if (ftHeader->validateMagic())
            {
                const Glyph *table                 = reinterpret_cast<const Glyph *>(fontBase + sizeof(FontHeader));
                const uint8_t *txOffset            = (fontBase + ftHeader->bitmapOffset + sizeof(FontHeader));
                Textures::TextureObject *ftTexture = Textures::openImageMemory(
                    ftHeader->id,
                    ftHeader->bitmapType,
                    txOffset,
                    ftHeader->bitmapLength);

                if (ftTexture)
                {
                    FontObject *ftObj = new FontObject(ftHeader->id, ftHeader, table, ftTexture);
                    fontBlobs[i]      = ftObj;
                }
                else
                {
                    LOG_FOBJ(LogErrorStrings::GenvCreateItemFailed,
                             ObjectStrings::TextureObject,
                             LogReasonStrings::GenvTypeIncompatible, familyName);
                    return deleteAndFail(fontBlobs, fsHeader);
                }
            }
            else
            {
                LOG_FOBJ(LogErrorStrings::GenvCreateItemFailed,
                         ObjectStrings::FontObject,
                         LogReasonStrings::GenvTypeIncompatible, familyName);
                return deleteAndFail(fontBlobs, fsHeader);
            }
        }

        if (!error)
        {
            FontsetObject *tempfs = new FontsetObject(*fsHeader, familyName, designerName, fontBlobs);
            if (tempfs->validateMagic()) return tempfs;
            return deleteAndFail(fontBlobs, fsHeader);
        }
        else
            return deleteAndFail(fontBlobs, fsHeader);
    }

    const Glyph FontObject::get(util::UTF8CodePoint id) const
    {
        auto entry = util::getHashTableEntry(
            _table,
            _header->numBuckets,
            id);

        if (entry)
            return *entry;
        else
            return get(FONT_INVALID_CHAR);
    }

    FontsetObject *loadFontsetFromFile(const char *filePath)
    {
        if (filePath == nullptr)
        {
            LOG_FOBJ(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvParameterError, 2);
            return nullptr;
        }

        // Textures::TextureObject *tObj = Textures::createTextureFromFile(, filePath);
        // if (!tObj) return FO_ERROR(GV_ERR_CREATE_FAILED);
        // return createFontFromMem(tObj);
        return nullptr;
    }

    FontsetObject *loadFontsetFromMem(const void *data, const size_t length)
    {
        // First param should be ignored
        if (auto p = testParams(1, (data != nullptr), (length >= sizeof(FontsetObject))))
        {
            LOG_FOBJ(LogErrorStrings::GenvCreateItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvParameterError, p);
            return nullptr;
        }

        size_t pos                = 0;
        const uint8_t *const base = static_cast<const uint8_t *>(data);
        const auto *fsHeader      = reinterpret_cast<const FontsetHeader *>(base + pos);
        pos += sizeof(FontsetHeader);

        if (fsHeader->magic != GENV_BITMAP_FONTSET_MAGIC || !fsHeader->fontCount)
            return nullptr;

        switch (fsHeader->version)
        {
        case 1: return loadFontsetV1(fsHeader, base, length);
        default: return nullptr;
        }
    }

    /*int Font::getCharacterWidth(util::UTF8CodePoint ch) const {
    auto header = metrics.as<FontMetricsHeader>();

    switch (ch) {
        case 0:
        case '\n':
        case '\r':
            return 0;

        case '\t':
            return header->tabWidth;

        case ' ':
            return header->spaceWidth;

        default:
            auto size = metrics.get(ch);

            return (size >> 16) & 0x7f;
    }
    }

    void Font::getStringBounds(
    const char *str,
    Rect       &rect,
    bool       wordWrap,
    bool       breakOnSpace
    ) const {
    if (!str || !metrics.ptr)
        return;

    auto header = metrics.as<FontMetricsHeader>();

    int x = rect.x1, maxX = rect.x1, y = rect.y1;

    for (;;) {
        auto ch   = util::parseUTF8Character(str);
        bool wrap = wordWrap;
        str      += ch.length;

        switch (ch.codePoint) {
            case 0:
                goto _break;

            case '\t':
                if (breakOnSpace)
                    goto _break;

                x += header->tabWidth;
                x -= x % header->tabWidth;
                break;

            case '\n':
                if (breakOnSpace)
                    goto _break;
                if (x > maxX)
                    maxX = x;

                x  = rect.x1;
                y += header->lineHeight;
                break;

            case '\r':
                if (breakOnSpace)
                    goto _break;
                if (x > maxX)
                    maxX = x;

                x = rect.x1;
                break;

            case ' ':
                if (breakOnSpace)
                    goto _break;

                x += header->spaceWidth;
                break;

            default:
                auto size = metrics.get(ch.codePoint);

                x   += (size >> 16) & 0x7f;
                wrap = false;
        }

        int boundaryX = rect.x2;

        if (wrap)
            boundaryX -= getStringWidth(str, true);

        if (x > boundaryX) {
            if (x > maxX)
                maxX = x;

            x  = rect.x1;
            y += header->lineHeight;
        }
        if (y > (rect.y2 - header->lineHeight))
            goto _break;
    }

    _break:
    rect.x2 = maxX;
    rect.y2 = y + header->lineHeight;
    }

    int Font::getStringWidth(const char *str, bool breakOnSpace) const {
    if (!str || !metrics.ptr)
        return 0;

    auto header = metrics.as<FontMetricsHeader>();

    int width = 0, maxWidth = 0;

    for (;;) {
        auto ch = util::parseUTF8Character(str);
        str    += ch.length;

        switch (ch.codePoint) {
            case 0:
                goto _break;

            case '\t':
                if (breakOnSpace)
                    goto _break;

                width += header->tabWidth;
                width -= width % header->tabWidth;
                break;

            case '\n':
            case '\r':
                if (breakOnSpace)
                    goto _break;
                if (width > maxWidth)
                    maxWidth = width;

                width = 0;
                break;

            case ' ':
                if (breakOnSpace)
                    goto _break;

                width += header->spaceWidth;
                break;

            default:
                width += (metrics.get(ch.codePoint) >> 16) & 0x7f;
        }
    }

    _break:
    if (width > maxWidth)
        maxWidth = width;

    return maxWidth;
    }

    int Font::getStringHeight(
    const char *str,
    int        width,
    bool       wordWrap,
    bool       breakOnSpace
    ) const {
    Rect _rect{
        .x1 = 0,
        .y1 = 0,
        .x2 = int16_t(width),
        .y2 = 0x7fff
    };

    getStringBounds(str, _rect, wordWrap, breakOnSpace);
    return _rect.y2;
    }*/

} // namespace Fonts