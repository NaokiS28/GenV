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

#include "font.hpp"

#include "common/objects/object.hpp"
#include "common/objects/texture.hpp"
#include "common/return_codes.hpp"
#include "common/util/string.hpp"

namespace Fonts
{
    FontObject::FontObject(util::Hash objectID)
        : ObjectBase(objectID)
    {
        setObjectType(GENV_FONT_OBJ_TYPENAME);
    }

    FontObject::FontObject(util::Hash objectID, FontHeader _header)
        : ObjectBase(objectID)
    {
        setObjectType(GENV_FONT_OBJ_TYPENAME);
        header = _header;
    }

    FontObject::FontObject(util::Hash objectID, FontHeader _header, Textures::TextureObject *_texture)
        : ObjectBase(objectID)
    {
        setObjectType(GENV_FONT_OBJ_TYPENAME);
        texture = _texture;
        header = _header;
    }

    FontObject::~FontObject()
    {
        if (texture)
            delete texture;
    }

    int loadFontFromFile(FontObject **fObj, const char *filePath)
    {
        if (filePath == nullptr)
        {
            LOG_FOBJ(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvParameterError, 2);
            return FO_ERROR(GV_ERR_INVALID_PARAM);
        }

        // Textures::TextureObject *tObj = Textures::createTextureFromFile(, filePath);
        // if (!tObj) return FO_ERROR(GV_ERR_CREATE_FAILED);
        // return createFontFromMem(tObj);
        return FO_ERROR(GV_ERR_UNSUPPORTED);
    }

    int loadFontFromMem(FontObject **fObj, const uint8_t *data, const size_t length)
    {
        // First param should be ignored
        if (auto p = testParams(1, data != nullptr, length))
        {
            LOG_FOBJ(LogErrorStrings::GenvCreateItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvParameterError, p);
            return FO_ERROR(GV_ERR_INVALID_PARAM);
        }

        return FO_ERROR(GV_ERR_UNSUPPORTED);
    }

    int loadFontFromMem(FontObject **fObj, FontHeader _header, Textures::TextureObject *tObj)
    {
        // First param should be ignored
        if (auto p = testParams(1, _header.validateMagic(), tObj != nullptr))
        {
            LOG_FOBJ(LogErrorStrings::GenvCreateItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvParameterError, p);
            return FO_ERROR(GV_ERR_INVALID_PARAM);
        }

        *fObj = new FontObject(_header.id, _header, tObj);
        if (!*fObj)
        {
            LOG_FOBJ(LogErrorStrings::GenvCreateItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvOutOfMemory);
            return FO_ERROR(GV_ERR_CREATE_FAILED);
        }

        return GV_OK;
    }

    const Glyph FontObject::get(util::UTF8CodePoint id) const
    {
        auto entry = util::getHashTableEntry(
            header.table,
            header.tableLength,
            id);

        if (entry)
            return *entry;
        else
            return get(FONT_INVALID_CHAR);
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