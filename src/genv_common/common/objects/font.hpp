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

#include "texture.hpp"

struct GlyphMetrics {
    uint16_t x = 0, y = 0, width = 0;
};

struct FontMetrics {
    enum : uint8_t {
        FONT_NONE,
        FONT_ITALIC,
        FONT_BOLD,
    } flags;
    int lineSpacing = 0;    // Adds/removes spacing between lines if called to do so.
    int kerning = 0;        // Kerning is only used for certain letters if the context calls for it.
};

class FontObject {
    protected:
        Textures::TextureObject *texture;
    
    public:
        FontObject(){}
        ~FontObject() = default;

        virtual int loadFontFromFile() = 0;
        virtual int loadFontFromMem(){}
};