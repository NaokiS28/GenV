/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * fontman.hpp - Created on 21-08-2025
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

#include "common/objects/font.hpp"
#include "common/objects/object.hpp"
#include "common/objects/texture.hpp"
#include "common/services/adminkey.hpp"
#include "common/util/hash.hpp"
#include "common/util/templates.hpp"
#include "common/return_codes.hpp"

#define MAX_FONTS 8
#define MAX_UPLOADED_FONTS 8

#define FM_ERROR(code) GV_ERROR(GV_SERVICE_VIDEO, "FontManager"_h, code)

namespace Fonts
{
    class FontManager
    {
    private:
        struct
        {
            util::Hash id  = 0; // Hashed ID
            uint8_t index  = 0; // Font list index
            uint16_t entry = 0; // Font blob entry index
            uint8_t size   = 0; // The set size
        } _currentFont;

        int _fontListLength                 = 0;
        FontsetObject *_fontList[MAX_FONTS] = {nullptr};
        util::PopList<util::Hash, MAX_FONTS> _uploadedFonts;

        int _loadFontset(FontsetObject *fObj);
        int _getFontIndex(util::Hash fontId);

    public:
        FontManager(AdminClass_Key key);

        int init();
        void shutdown();

        inline FontObject *getCurrentFont()
        {
            return &_fontList[_currentFont.entry]->fontAt(_currentFont.index);
        }
        FontObject *getFont(util::Hash fontsetId, uint8_t size, uint8_t flags = FONT_NONE);
        int setFont(util::Hash fontsetId, uint8_t size, uint8_t flags = FONT_NONE);

        int unloadFont(util::Hash fontId);

        int loadFontsetFromFile(const char *filePath);
        int loadFontsetFromMemory(const uint8_t *data, const size_t length);
    };
} // namespace Fonts