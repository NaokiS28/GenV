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
#include "common/objects/texture.hpp"
#include "common/services/adminkey.hpp"
#include "common/util/hash.hpp"
#include "vendor/printf.h"
#include "common/return_codes.hpp"

#define MAX_FONTS 8

#define FM_ERROR(code) GV_ERROR(GV_SERVICE_VIDEO, "FontManager"_h, code)

namespace Fonts
{
    class FontManager
    {
    private:
        int currentFontIdx = 0;
        int fontListLength = 0;
        FontObject *fontList[MAX_FONTS] = {nullptr};

        int _loadFont(FontObject *fObj);

    public:
        FontManager(AdminClass_Key key);

        int init();
        void shutdown();

        inline const FontObject *getCurrentFont()
        {
            return fontList[currentFontIdx];
        }
        FontObject *getFont(uint8_t idx);
        FontObject *getFont(util::Hash idx);

        int loadFontFromFile(const char *filePath);
        int loadFontFromMemory(const uint8_t *data, const size_t length, const FontHeader header);
        int loadFontFromMemory(const FontObject &font);

        int setFont(uint8_t idx);
        int setFont(util::Hash fontId);

        int unloadFontAt(uint8_t idx);
        int unloadFont(util::Hash id);
    };
} // namespace Fonts