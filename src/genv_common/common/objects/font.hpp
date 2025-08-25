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

#pragma once

#include <stdint.h>

#include "texture.hpp"
#include "common/util/hash.hpp"
#include "common/services/video/color.hpp"

namespace Fonts
{
    constexpr const Video::Color font_shadow(0, 0, 0, 128);

    struct GlyphMetrics
    {
        uint16_t c = '\0', x = 0, y = 0, width = 0;
    };

    enum FontFlags : uint8_t
    {
        FONT_NONE,
        FONT_ITALIC,
        FONT_BOLD,
    };

    struct FontMetrics
    {
        const char *name = nullptr;
        const char *designer = nullptr;
        const util::Hash id = 0;
        const uint8_t flags = 0;

        const GlyphMetrics *charTable = nullptr;
        const int tableSize = 0;

        const int height = 0;      // Font height
        const int lineSpacing = 0; // Adds/removes spacing between lines if called to do so.
        const int kerning = 0;     // Kerning is only used for certain letters if the context calls for it.
    };

    constexpr FontMetrics makeFontInfo(
        const char *name, const char *maker, int size,
        const GlyphMetrics *table, const int tsize,
        const uint8_t _flags = 0, const int _spacing = 0, const int _kerning = 0)
    {
        return FontMetrics{
            name, maker,
            (util::hash(name, -1, 0) + util::hash(maker, -1, 0)),
            _flags,
            table, tsize,
            size, _spacing, _kerning};
    }
} // namespace Fonts

class FontObject
{
protected:
    Fonts::FontMetrics *metrics = nullptr;
    Textures::TextureObject *texture = nullptr;

public:
    FontObject(util::Hash objectID);
    FontObject(util::Hash objectID, const char *filePath);
    FontObject(util::Hash objectID, Fonts::FontMetrics *_metrics, Textures::TextureObject *_texture);
    ~FontObject();

    virtual int loadFontFromFile(const char *filePath);
    virtual int loadFontFromMem();
};